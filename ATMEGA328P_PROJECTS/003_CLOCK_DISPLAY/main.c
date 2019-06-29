/*
 * 003_CLOCK_DISPLAY.c
 *
 * Created: 14.06.2019 22:02:55
 * Author : Aliaksandr
 */

#include <avr/io.h>
#include "TM1637/TM1637.h"

void delay(uint32_t ms)
{   
    for (uint32_t i=0; i < (1000 + ms); i++)
    {
        asm("NOP\n");
    }                    
}

int main(void)
{
    
    TM1637_init(&DDRC, &PORTC, 0, &DDRC, &PORTC, &PINC, 1);

    
    
    while (1)
    {
        
    }
}

