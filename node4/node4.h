#ifndef NODE4_H
#define NODE4_H

// Delay
void delay_ms(unsigned int ms);

// LCD
void lcd_cmd(unsigned char c);
void lcd_data(unsigned char d);
void lcd_init(void);

// CAN
void can_init(void);
int  can_available(void);
unsigned int can_read_id(void);
unsigned char can_read_data(void);

// Controls
void control_init(void);
void motor_on(void);
void motor_off(void);
void light_on(void);
void light_off(void);

#endif
