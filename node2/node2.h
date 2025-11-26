#ifndef NODE2_H
#define NODE2_H

void delay_ms(unsigned int ms);

void lcd_cmd(unsigned char c);
void lcd_data(unsigned char d);
void lcd_init(void);

void can_init(void);
void can_send(unsigned int id, unsigned char d);

void ldr_init(void);
unsigned int ldr_read(void);

#endif
