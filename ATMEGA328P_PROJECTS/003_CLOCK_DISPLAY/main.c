/*
 * 003_CLOCK_DISPLAY.c
 *
 * Created: 14.06.2019 22:02:55
 * Author : Aliaksandr
 */

#include <avr/io.h>
#include "TM1637/TM1637.h"

int main(void)
{
    TM1637_init(&DDRC, &PORTC, 0, &DDRC, &PORTC, 1);
    while (1)
    {
    }
}

