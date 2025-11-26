#include <LPC21xx.h>
#include "node4.h"

/* ----------------------- DELAY ------------------------- */
void delay_ms(unsigned int ms)
{
    T0PR  = 15000 - 1;
    T0TCR = 0x01;
    while(T0TC < ms);
    T0TCR = 0x03;
    T0TCR = 0x00;
}

/* ----------------------- LCD --------------------------- */
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

/* ----------------------- CAN --------------------------- */
void can_init(void)
{
    PINSEL1 |= (1<<18) | (1<<20);  // TXD1 / RXD1
    C1MOD  = 1;
    C1BTR  = 0x001C001D;           // ~125kbps
    C1MOD  = 0;
}

int can_available(void)
{
    return (C1GSR & 1);   // RBS bit
}

unsigned int can_read_id(void)
{
    return C1RID;
}

unsigned char can_read_data(void)
{
    return (unsigned char)C1RDA;
}

/* --------------------- MOTOR/LIGHT CONTROL -------------- */
void control_init(void)
{
    IODIR0 |= (1<<7) | (1<<8);   // P0.7 motor, P0.8 light
}

void motor_on(void)  { IO0SET = (1<<7); }
void motor_off(void) { IO0CLR = (1<<7); }

void light_on(void)  { IO0SET = (1<<8); }
void light_off(void) { IO0CLR = (1<<8); }

/* ------------------------- MAIN -------------------------- */
int main()
{
    unsigned int id;
    unsigned char data;

    // LCD pins
    IODIR1 |= 0x00FF0000 | (1<<16) | (1<<17);

    lcd_init();
    control_init();
    can_init();

    while(1)
    {
        if(can_available())
        {
            id   = can_read_id();
            data = can_read_data();
            C1CMR = (1<<2);  // release receive buffer

            lcd_cmd(0x01);   // clear LCD
            lcd_cmd(0x80);

            /* ----------------- NODE 1 → OBSTACLE ----------------- */
            if(id == 1 && data == 1)
            {
                motor_off();
                light_on();
                lcd_data('S'); lcd_data('T'); lcd_data('O'); lcd_data('P');
            }

            /* ----------------- NODE 2 → DARK --------------------- */
            else if(id == 2 && data == 1)
            {
                light_on();
                lcd_data('D'); lcd_data('A'); lcd_data('R'); lcd_data('K');
            }

            /* ----------------- NODE 3 → AUTH OK ------------------ */
            else if(id == 3 && data == 1)
            {
                motor_on();
                lcd_data('O'); lcd_data('K');
            }

            delay_ms(800);
        }
    }
}
