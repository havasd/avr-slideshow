#include <stdio.h>

#include "buttonsim.h"
#include "charmap.h"
#include "lcdsim.h"
#include "utils.h"

#include <stdint.h>
#include <unistd.h>

#define MAX_LEN 1024
#define LCD_WIDTH 16

static unsigned char pattern_map[16];

// Key: Pattern
// Value: Offset in CG RAM or character in ROM
void init_pattern_map()
{
    pattern_map[0] = ' ';
    for (int i = 1; i < 16; ++i)
        pattern_map[i] = '?';
}

// Bubble short pattern frequencies
// Skip zero (0x0): space character is already stored in ROM
void sort_patterns(unsigned char patterns[16], int pattern_freqs[16])
{
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 14; ++j) {
            unsigned char a = pattern_freqs[patterns[j]];
            unsigned char b = pattern_freqs[patterns[j + 1]];

            if (a < b) {
                unsigned char temp = patterns[j + 1];
                patterns[j + 1] = patterns[j];
                patterns[j] = temp;
            }
        }
    }
}

// Try to keep pointer of pattern in CG RAM
void fill_cg_buffer(unsigned char cg_buffer[8], unsigned char patterns[16])
{
    // Each bit represents a pattern. If bit is 1 the pattern is already stored.
    unsigned short stored_mask = 0;

    // Each bit represents a memory slot in CG RAM. If bit is 1 the slot is reserved.
    unsigned char reserved_mask = 0;

    // Mark patterns that are already stored in CG RAM
    for (int i = 0; i < 8; ++i) {
        unsigned char pattern = patterns[i];
        unsigned char cg_pos = pattern_map[pattern];

        // Pattern is already in CG RAM
        if (cg_pos < 8) {
            cg_buffer[cg_pos] = pattern;
            stored_mask |= 1 << pattern;
            reserved_mask |= 1 << cg_pos;
        }
    }

    // Add new patterns to CG RAM
    for (int i = 0; i < 8; ++i) {
        unsigned char pattern = patterns[i];

        // Pattern is already stored
        if (stored_mask & (1 << pattern))
            continue;

        // Find empty slot in CG RAM
        unsigned char cg_pos = 0;
        for (; cg_pos < 8; ++cg_pos) {
            if (~reserved_mask & (1 << cg_pos))
                break;

            // No more free slot
            if (cg_pos == 7) {
                //lcd_send_line1("!!! ERROR !!!");
                //lcd_send_line2("CG RAM BUFFER FULL");
                fprintf(stderr, "CG RAM BUFFER ERROR\n");
                return;
            }
        }

        cg_buffer[cg_pos] = pattern;
        reserved_mask |= 1 << cg_pos;
    }
}

// Load first 8 patterns to the CG RAM (if non-zero!)
void update_cg_ram(unsigned char cg_buffer[8], int pattern_freqs[16])
{
    for (int i = 0; i < 8; ++i) {
        unsigned char pattern = cg_buffer[i];
        if (!pattern_freqs[pattern])
            break;

        // Pattern is already in CG RAM
        if (cg_buffer[i] == pattern_map[i])
            continue;

        pattern_map[pattern] = i;

        lcd_send_command(CG_RAM_ADDR + i*8);
        // Pattern size is 4bit
        for (int j = 0; j < 4; ++j) {
            unsigned char row = 0x00;
            if ((pattern >> j) & 1)
                row = 0xff;
            lcd_send_data(row);
            lcd_send_data(row);
        }
    }
}

void show(unsigned char *line0_buffer, unsigned char *line1_buffer, int buffer_index, int line_len)
{
    unsigned char line0_shadow[LCD_WIDTH];
    unsigned char line1_shadow[LCD_WIDTH];

    int pattern_freqs[16] = { 0, 0, 0, 0,
                              0, 0, 0, 0,
                              0, 0, 0, 0,
                              0, 0, 0, 0 };

    for (int i = 0; i < MIN(LCD_WIDTH, line_len); ++i, ++buffer_index) {
        if (buffer_index >= line_len)
            buffer_index = 0;

        line0_shadow[i] = line0_buffer[buffer_index];
        line1_shadow[i] = line1_buffer[buffer_index];

        pattern_freqs[line0_shadow[i]]++;
        pattern_freqs[line1_shadow[i]]++;
    }

    unsigned char patterns[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x0 };
    sort_patterns(patterns, pattern_freqs);

    unsigned char cg_buffer[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    fill_cg_buffer(cg_buffer, patterns);

    update_cg_ram(cg_buffer, pattern_freqs);

    // TODO(pvarga): Replace unloaded but used patterns by the most most similar loaded pattern in shadow memory

    lcd_send_command(DD_RAM_ADDR);
    for (int i = 0; i < LCD_WIDTH; ++i)
        lcd_send_data(pattern_map[line0_shadow[i]]);

    lcd_send_command(DD_RAM_ADDR2);
    for (int i = 0; i < LCD_WIDTH; ++i)
        lcd_send_data(pattern_map[line1_shadow[i]]);

    lcd_sim_print();
}

int main(void)
{
    lcd_init();
    button_sim_init();

    const char *message = "ABCDEF";

    const int buffer_size = MAX_LEN * (CHAR_WIDTH + 1);
    unsigned char line0_buffer[buffer_size];
    unsigned char line1_buffer[buffer_size];

    for (int i = 0; i < buffer_size; ++i) {
        line0_buffer[i] = 0;
        line1_buffer[i] = 0;
    }

    // Cut the string above the maximum
    int len = MIN(strlen(message), MAX_LEN);

    for (int i = 0; i < len; ++i) {
        int ch = (int)message[i];
        for (int j = 0; j < CHAR_WIDTH; ++j) {
            unsigned char col0 = charmap[ch][j] & 0b1111;
            unsigned char col1 = (charmap[ch][j] >> 4) & 0b1111;

            int index = (CHAR_WIDTH + 1) * i + j;
            line0_buffer[index] = col0;
            line1_buffer[index] = col1;
        }

        // Insert space between characters
        int index = (CHAR_WIDTH + 1) * i + CHAR_WIDTH;
        line0_buffer[index] = 0x0;
        line1_buffer[index] = 0x0;
    }

    init_pattern_map();

    // Add extra whitespace to the end
    const int line_len = len * (CHAR_WIDTH + 1) + 2;
    int buffer_index = 0;
    int enable_slide = 1;

    uint16_t cycle = 0;

    lcd_send_line1("  AVR SLIDESHOW");
    lcd_send_line2("  by Peter Varga");
    while (cycle++ < 100 && !button_pressed()) {
        lcd_delay(1);
        button_unlock();
    }

    cycle = 0;
    while (cycle++ < UINT16_MAX) {
        if (cycle >= 65000U)
            cycle = 0;

        int button = button_pressed();
        if (button == BUTTON_CENTER)
            enable_slide ^= 1;
        if (button == BUTTON_QUIT)
            break;

        if (enable_slide && cycle % 1000 == 0) {
            if (++buffer_index >= line_len)
                buffer_index = 0;

            show(&line0_buffer[0], &line1_buffer[0], buffer_index, line_len);
            lcd_delay(10);
        }

        button_unlock();
    }

    button_sim_terminate();
    lcd_sim_terminate();

    return 0;
}

