/*
 * 003_CLOCK_DISPLAY.c
 *
 * Created: 14.06.2019 22:02:55
 * Author : Aliaksandr
 */

#define F_CPU 16000000UL
#include <util/delay.h>

#include "TM1637/TM1637.h"
#include "PIN_MAP/pin_map.h"
#include "PIN_MAP/avr_map.h"  // includes <avr/io.h>

int main(void)
{
    TM1637_init(&PC0, &PC1);

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

