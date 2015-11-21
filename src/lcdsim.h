#ifndef LCDSIM_H
#define LCDSIM_H

#define CG_RAM_SIZE  128 * 8
#define DD_RAM_SIZE  16
#define DD_RAM_SIZE2 16

#define CG_RAM_ADDR  0
#define DD_RAM_ADDR  CG_RAM_SIZE
#define DD_RAM_ADDR2 DD_RAM_ADDR + DD_RAM_SIZE

void lcd_init();
void lcd_send_command(int command);
void lcd_send_data(unsigned char data);

// TODO: Add lcd_delay

void lcd_sim_print();

#endif // LCDSIM_H

