#ifndef NODE3_H
#define NODE3_H

// Delay
void delay_ms(unsigned int ms);

// LCD
void lcd_cmd(unsigned char c);
void lcd_data(unsigned char d);
void lcd_init(void);

// CAN
void can_init(void);
void can_send(unsigned int id, unsigned char d);

// RFID Sensor (Pushbutton)
void rfid_init(void);
int  rfid_read(void);

#endif
