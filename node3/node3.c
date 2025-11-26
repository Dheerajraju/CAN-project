#include <LPC21xx.h>
#include "node3.h"

/* --------------------- DELAY FUNCTION --------------------- */
void delay_ms(unsigned int ms)
{
    T0PR  = 15000 - 1;
    T0TCR = 0x01;
    while(T0TC < ms);
    T0TCR = 0x03;
    T0TCR = 0x00;
}

/* ------------------------ LCD ----------------------------- */
void lcd_cmd(unsigned char c)
{
    IO1CLR = 0x00FF0000;
    IO1SET = (c << 16);

    IO1CLR = (1<<16); // RS = 0
    IO1SET = (1<<17); // EN = 1
    delay_ms(2);
    IO1CLR = (1<<17);
}

void lcd_data(unsigned char d)
{
    IO1CLR = 0x00FF0000;
    IO1SET = (d << 16);

    IO1SET = (1<<16); // RS = 1
    IO1SET = (1<<17);
    delay_ms(2);
    IO1CLR = (1<<17);
}

void lcd_init(void)
{
    delay_ms(20);
    lcd_cmd(0x38);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
}

/* ------------------------ CAN ----------------------------- */
void can_init(void)
{
    PINSEL1 |= (1<<18) | (1<<20);  // TXD1 / RXD1
    C1MOD  = 1;
    C1BTR  = 0x001C001D;           // ~125kbps
    C1MOD  = 0;
}

void can_send(unsigned int id, unsigned char d)
{
    C1TID1 = id;
    C1TDA1 = d;
    C1TFI1 = (1<<16);
    C1CMR  = 0x21;
}

/* --------------------- RFID (BUTTON) ---------------------- */
void rfid_init(void)
{
    IODIR0 &= ~(1<<4); // P0.4 input
}

int rfid_read(void)
{
    if(!(IO0PIN & (1<<4))) // active LOW
        return 1;
    else
        return 0;
}

/* ------------------------ MAIN ----------------------------- */
int main()
{
    // LCD pins
    IODIR1 |= 0x00FF0000 | (1<<16) | (1<<17);

    delay_ms(20);
    lcd_init();
    rfid_init();
    can_init();

    while(1)
    {
        // Display RFID status
        lcd_cmd(0x80);
        lcd_data('R');
        lcd_data('F');
        lcd_data('I');
        lcd_data('D');

        // If button pressed → authorized
        if(rfid_read())
        {
            can_send(3, 1);  // ID = 3, AUTH = 1

            lcd_cmd(0xC0);
            lcd_data('O'); 
            lcd_data('K');
            delay_ms(800);
        }

        delay_ms(200);
    }
}
