#include <LPC21xx.h>
#include "node2.h"

/* ----------------------------------------------------------
   MANUAL ADC REGISTER DEFINITIONS (to fix AD0CR undefined error)
   ---------------------------------------------------------- */
#define AD0CR   (*((volatile unsigned long*)0xE0034000))
#define AD0DR1  (*((volatile unsigned long*)0xE0034014))

/* ----------------------- DELAY -------------------------- */
void delay_ms(unsigned int ms)
{
    T0PR  = 15000 - 1;
    T0TCR = 0x01;
    while(T0TC < ms);
    T0TCR = 0x03;
    T0TCR = 0x00;
}

/* ----------------------- LCD ---------------------------- */
void lcd_cmd(unsigned char c)
{
    IO1CLR = 0x00FF0000;
    IO1SET = (c << 16);

    IO1CLR = (1<<16);
    IO1SET = (1<<17);
    delay_ms(2);
    IO1CLR = (1<<17);
}

void lcd_data(unsigned char d)
{
    IO1CLR = 0x00FF0000;
    IO1SET = (d << 16);

    IO1SET = (1<<16);
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

/* ----------------------- CAN ---------------------------- */
void can_init(void)
{
    PINSEL1 |= (1<<18) | (1<<20);  // P0.25 = TXD1, P0.26 = RXD1
    C1MOD = 1;
    C1BTR = 0x001C001D;
    C1MOD = 0;
}

void can_send(unsigned int id, unsigned char d)
{
    C1TID1 = id;
    C1TDA1 = d;
    C1TFI1 = (1<<16);
    C1CMR = 0x21;
}

/* ----------------------- LDR / ADC ----------------------- */
void ldr_init(void)
{
    PINSEL1 |= (1<<24);     // P0.28 = AD0.1
    AD0CR = (1<<1) |        // Select ADC channel 1
            (4<<8) |        // Clock divider
            (1<<21);        // ADC Power on
}

unsigned int ldr_read(void)
{
    AD0CR |= (1<<24);            // Start conversion
    while(!(AD0DR1 & (1<<31)));  // Wait for DONE
    return (AD0DR1 >> 6) & 0x3FF;  // 10-bit result
}

/* ------------------------ MAIN --------------------------- */
int main()
{
    unsigned int light;

    IODIR1 |= 0x00FF0000 | (1<<16) | (1<<17);

    lcd_init();
    ldr_init();
    can_init();

    while(1)
    {
        light = ldr_read(); // Read LDR value

        lcd_cmd(0x80);
        lcd_data((light/100)+'0');
        lcd_data(((light/10)%10)+'0');
        lcd_data((light%10)+'0');

        if(light < 400)
        {
            can_send(2, 1);    // Send DARK condition
        }

        delay_ms(300);
    }
}
