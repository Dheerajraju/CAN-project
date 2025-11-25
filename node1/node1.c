#include <LPC21xx.h>
#include "node1.h"

/* ------------------------- DELAY -------------------------- */
void delay_ms(unsigned int ms)
{
    T0PR  = 15000 - 1;  // Your delay setting
    T0TCR = 0x01;
    while(T0TC < ms);
    T0TCR = 0x03;
    T0TCR = 0x00;
}

/* ------------------------- LCD ---------------------------- */
void lcd_cmd(unsigned char c)
{
    IO1CLR = 0x00FF0000;
    IO1SET = (c << 16);

    IO1CLR = (1<<16);     // RS=0
    IO1SET = (1<<17);     // EN=1
    delay_ms(2);
    IO1CLR = (1<<17);     // EN=0
}

void lcd_data(unsigned char d)
{
    IO1CLR = 0x00FF0000;
    IO1SET = (d << 16);

    IO1SET = (1<<16);     // RS=1
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

/* -------------------------- CAN --------------------------- */
void can_init(void)
{
    PINSEL1 |= (1<<18) | (1<<20);   // P0.25 TXD1, P0.26 RXD1
    C1MOD = 1;
    C1BTR = 0x001C001D;             // approx 125 kbps
    C1MOD = 0;
}

void can_send(unsigned int id, unsigned char d)
{
    C1TID1 = id;
    C1TDA1 = d;
    C1TFI1 = (1<<16);
    C1CMR = 0x21;                   // send frame
}

/* ---------------------- ULTRASONIC ------------------------ */
unsigned int get_distance(void)
{
    unsigned int t;

    // Trigger pulse
    IO0CLR = (1<<10);
    delay_ms(1);
    IO0SET = (1<<10);
    delay_ms(1);
    IO0CLR = (1<<10);

    // Wait for ECHO high
    t = 50000;
    while(!(IO0PIN & (1<<11)))
        if(--t == 0) return 999;

    // Measure HIGH duration using Timer1
    T1PR = 14;
    T1TCR = 2;      // reset
    T1TCR = 1;      // start

    t = 50000;
    while(IO0PIN & (1<<11))
        if(--t == 0) break;

    T1TCR = 0;      // stop

    return T1TC / 58;  // Convert to cm
}

/* --------------------------- MAIN -------------------------- */
int main()
{
    unsigned int dist;

    // LCD pins
    IODIR1 |= 0x00FF0000 | (1<<16) | (1<<17);

    // Ultrasonic pins
    IODIR0 |= (1<<10);       // TRIG
    IODIR0 &= ~(1<<11);      // ECHO input

    lcd_init();
    can_init();

    while(1)
    {
        dist = get_distance();

        lcd_cmd(0x80);
        lcd_data((dist/100)+'0');
        lcd_data(((dist/10)%10)+'0');
        lcd_data((dist%10)+'0');

        if(dist < 20)
        {
            can_send(1, 1);     // ID = 1 (obstacle)
            lcd_cmd(0xC0);
            lcd_data('O'); lcd_data('B'); lcd_data('S');
        }

        delay_ms(200);
    }
}
