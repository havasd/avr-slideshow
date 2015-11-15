#include <stdio.h>

#include "charmap.h"
#include "lcdsim.h"

static unsigned char CHAR_E[8] =
    { 0x00, 0x00,
      0xff, 0xff,
      0xff, 0xff,
      0xff, 0xff };

static unsigned char CHAR_9[8] =
    { 0xff, 0xff,
      0x00, 0x00,
      0x00, 0x00,
      0xff, 0xff };

static unsigned char CHAR_F[8] =
    { 0xff, 0xff,
      0xff, 0xff,
      0xff, 0xff,
      0xff, 0xff };

static unsigned char CHAR_1[8] =
    { 0xff, 0xff,
      0x00, 0x00,
      0x00, 0x00,
      0x00, 0x00 };

static unsigned char CHAR_6[8] =
    { 0x00, 0x00,
      0xff, 0xff,
      0xff, 0xff,
      0x00, 0x00 };

static unsigned char CHAR_7[8] =
    { 0xff, 0xff,
      0xff, 0xff,
      0xff, 0xff,
      0x00, 0x00 };

static unsigned char CHAR_8[8] =
    { 0x00, 0x00,
      0x00, 0x00,
      0x00, 0x00,
      0xff, 0xff };

int main(void)
{
    lcd_init();

    lcd_send_command(CG_RAM_ADDR);
    for (int i = 0; i < 8; ++i)
        lcd_send_data(CHAR_E[i]);
    for (int i = 0; i < 8; ++i)
        lcd_send_data(CHAR_9[i]);
    for (int i = 0; i < 8; ++i)
        lcd_send_data(CHAR_F[i]);
    for (int i = 0; i < 8; ++i)
        lcd_send_data(CHAR_1[i]);
    for (int i = 0; i < 8; ++i)
        lcd_send_data(CHAR_6[i]);
    for (int i = 0; i < 8; ++i)
        lcd_send_data(CHAR_7[i]);
    for (int i = 0; i < 8; ++i)
        lcd_send_data(CHAR_8[i]);


    lcd_send_command(DD_RAM_ADDR);
    lcd_send_data(0); // 0xE
    lcd_send_data(1); // 0x9
    lcd_send_data(0); // 0xE

    lcd_send_data(' ');

    lcd_send_data(2); // 0xF
    lcd_send_data(1); // 0x9
    lcd_send_data(4); // 0x6

    lcd_send_data(' ');

    lcd_send_data(0); // 0xE
    lcd_send_data(3); // 0x1
    lcd_send_data(3); // 0x1

    lcd_send_command(DD_RAM_ADDR2);
    lcd_send_data(2); // 0xF
    lcd_send_data(3); // 0x1
    lcd_send_data(2); // 0xF

    lcd_send_data(' ');

    lcd_send_data(2); // 0xF
    lcd_send_data(1); // 0x9
    lcd_send_data(4); // 0x6

    lcd_send_data(' ');

    lcd_send_data(5); // 0x7
    lcd_send_data(6); // 0x8
    lcd_send_data(6); // 0x8

    lcd_sim_print();

    return 0;
}

