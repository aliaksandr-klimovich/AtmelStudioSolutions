/*
* 02_LED_SWITCH_BUTTON_INT.c
*
* Created: 12/16/2018 11:19:29 AM
* Author : Aliaksandr
*/

#include <avr/io.h>

int main(void)
{

    DDRB = (1 << DDB0) | (1 << DDB4);
    PORTB = (1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB4);

    while (1) {
        if (!(PINB & (1 << PINB1))) {   // if KEY1 is pressed
            PORTB &= ~(1 << PORTB0);    // enable LED1
        } else {
            PORTB |= (1 << PORTB0);
        }

        if (PINB & (1 << PINB2)) {      // if KEY2 is depressed
            PORTB &= ~(1 << PORTB4);    // enable LED2
        } else {
            PORTB |= (1 << PORTB4);
        }
    }
}

