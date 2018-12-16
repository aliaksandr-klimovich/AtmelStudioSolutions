/*
* 01_LED_SWITCH_ON_TOV.c
*
* Created: 12/10/2018 9:34:57 PM
* Author : Aliaksandr
*/

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned int c = 0;

ISR(TIM0_OVF_vect) {
	if (++c == 2285 * 2) {  // 2.285[kHz] taken from multimeter
		PINB = (1 << PINB0);
		c = 0;
	}
}

int main(void)
{
	DDRB = (1 << DDB0);
	PORTB = (1 << PORTB0);
	
	TIMSK0 = (1 << TOIE0);
	TCCR0B = (1 << CS00);
	
	sei();
	
	while (1)
	{
	}
}

