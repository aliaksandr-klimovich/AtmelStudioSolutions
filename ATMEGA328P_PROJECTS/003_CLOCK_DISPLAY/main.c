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
    TM1637_init(&DDRC, &PORTC, 0, &DDRC, &PORTC, &PINC, 1, 100);
    
    uint8_t data[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    TM1637_write_SRAM_auto_increment(
        TM1637_CMD_DATA_WRITE, 
        TM1637_CMD_INIT_ADDR, 
        TM1637_CMD_BRIGHTNESS | 0x04,
        data,
        6
    );
    
    while (1)
    {
    }
}

