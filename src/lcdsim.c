#include "lcdsim.h"

#include "stdio.h"
#include "stdlib.h"

#define RAM_SIZE CG_RAM_SIZE + DD_RAM_SIZE + DD_RAM_SIZE2
static unsigned char lcd_ram[RAM_SIZE];
static unsigned char *ram_ptr;

void lcd_init()
{
    printf("%cc", 27);
    printf("\n");

    for (int i = 0; i < RAM_SIZE; ++i)
        lcd_ram[i] = 0;

    ram_ptr = 0;
}

void lcd_send_command(int command)
{
    if (command < 0 || command >= RAM_SIZE) {
        fprintf(stderr, "Invalid command: 0x%x\n", command);
        exit(1);
    }

    ram_ptr = &lcd_ram[command];
}

void lcd_send_data(unsigned char data) {
    if (!ram_ptr) {
        fprintf(stderr, "LCD command has not been sent!\n");
        exit(1);
    }

    if ((void *)ram_ptr >= (void *)(&lcd_ram + RAM_SIZE)) {
        fprintf(stderr, "Memory Overflow!");
        exit(1);
    }

    *ram_ptr = data;
    ram_ptr++;
}

void lcd_sim_print_line(unsigned char *dd_ram, int dd_ram_size)
{
    for (int row = 0; row < 8; ++row) {
        for (int i = 0; i < dd_ram_size; ++i) {
            unsigned char ch = dd_ram[i];
            unsigned char pattern = lcd_ram[CG_RAM_ADDR + ch * 8 + row];
            for (int s = 7; s > 2; --s) {
                if ((pattern >> s) & 1)
                    putchar('*');
                else
                    putchar(' ');
            }
            putchar(' ');
        }
        putchar('\n');
    }

}

void lcd_sim_print()
{
    // Save current cursor position
    printf("%c[s", 27);
    lcd_sim_print_line(&lcd_ram[DD_RAM_ADDR], DD_RAM_SIZE);
    putchar('\n');
    lcd_sim_print_line(&lcd_ram[DD_RAM_ADDR2], DD_RAM_SIZE2);
    putchar('\n');
    // Restore cursor position
    printf("%c[u", 27);
}
