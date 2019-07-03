/*
 * 003_CLOCK_DISPLAY.c
 *
 * Created: 14.06.2019 22:02:55
 * Author : Aliaksandr
 */

#include <avr/io.h>

#define F_CPU 16000000UL
#include <util/delay.h>

#include "TM1637/TM1637.h"


int main(void)
{
    TM1637_init(&DDRC, &PORTC, 0, &DDRC, &PORTC, &PINC, 1);
    TM1637_screen_on = 1;

    while (1)
    {
        for (uint8_t min=0; min<60; min++)
        {
            for (uint8_t sec=0; sec<60; sec++)
            {
                TM1637_print("%02u:%02u", min, sec);
                _delay_ms(500U);
                TM1637_print("%02u_%02u", min, sec);
                _delay_ms(500U);
            }
        }
    }
}

