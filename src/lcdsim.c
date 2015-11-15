#include "lcdsim.h"

#include "stdio.h"
#include "stdlib.h"

static unsigned char cg_ram[CG_RAM_SIZE];
static unsigned char dd_ram[DD_RAM_SIZE];
static unsigned char dd_ram2[DD_RAM_SIZE2];

static int base;
static int offset;

void lcd_init()
{
    for (int i = 0; i < CG_RAM_SIZE; ++i)
        cg_ram[i] = 0;
    for (int i = 0; i < DD_RAM_SIZE; ++i)
        dd_ram[i] = ' ';
    for (int i = 0; i < DD_RAM_SIZE2; ++i)
        dd_ram2[i] = ' ';

    base = -1;
    offset = 0;
}

void lcd_send_command(int command)
{
    if (command >= CG_RAM_ADDR && command < CG_RAM_ADDR + CG_RAM_SIZE) {
        base = CG_RAM_ADDR;
        offset = command - CG_RAM_ADDR;
        return;
    }

    if (command >= DD_RAM_ADDR && command < DD_RAM_ADDR + DD_RAM_SIZE) {
        base = DD_RAM_ADDR;
        offset = command - DD_RAM_ADDR;
        return;
    }

    if (command >= DD_RAM_ADDR2 && command < DD_RAM_ADDR2 + DD_RAM_SIZE2) {
        base = DD_RAM_ADDR2;
        offset = command - DD_RAM_ADDR2;
        return;
    }

    fprintf(stderr, "Invalid command: 0x%x\n", command);
    exit(1);
}

void lcd_send_data(unsigned char data) {
    if (base < 0) {
        fprintf(stderr, "LCD command has not been sent!\n");
        exit(1);
    }

    unsigned char *ram = 0;

    switch (base) {
    case CG_RAM_ADDR:
        ram = (void *)&cg_ram;
        offset %= 8 * 8;
        break;
    case DD_RAM_ADDR:
        ram = (void *)&dd_ram;
        offset %= DD_RAM_SIZE;
        break;
    case DD_RAM_ADDR2:
        ram = (void *)&dd_ram2;
        offset %= DD_RAM_SIZE2;
        break;
    default:
        fprintf(stderr, "Invalid base pointer: 0x%x\n", base);
        exit(1);
    }

    ram[offset++] = data;
}

void lcd_sim_print_line(unsigned char *ram, int ram_size)
{
    for (int row = 0; row < 8; ++row) {
        for (int i = 0; i < ram_size; ++i) {
            unsigned char ch = ram[i];
            unsigned char pattern = cg_ram[ch * 8 + row];
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
    lcd_sim_print_line(dd_ram, DD_RAM_SIZE);
    printf("\n");
    lcd_sim_print_line(dd_ram2, DD_RAM_SIZE2);
}


