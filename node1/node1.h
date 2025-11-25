#ifndef NODE1_H
#define NODE1_H

// Delay
void delay_ms(unsigned int ms);

// LCD
void lcd_cmd(unsigned char c);
void lcd_data(unsigned char d);
void lcd_init(void);

// CAN
void can_init(void);
void can_send(unsigned int id, unsigned char d);

// Ultrasonic
unsigned int get_distance(void);

#endif
