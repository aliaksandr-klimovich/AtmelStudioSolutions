#include <avr/io.h>

#include "buzzer.h"


void buzzer_init(Buzzer *buzzer)
{
    DDR_SET_P(buzzer->dio);
    PORTR_SET_P(buzzer->dio);

    buzzer_disable(buzzer);

    // Set clear timer on compare match (CTC) mode
    TCCR2A = (1 << WGM21);

    // Configure tone
    OCR2A = 0x1F;  // Output compare match register
    TCCR2B = (1 << CS22);  // CLK/128 (from prescaler)

}

void buzzer_disable(Buzzer *buzzer)
{
    // Disconnect output compare unit
    TCCR2A &= ~(1 << COM2B0);
    buzzer->enabled = 0;
}

void buzzer_enable(Buzzer *buzzer)
{
    // Toggle OC2A on compare match

    TCCR2A |= (1 << COM2B0);
    buzzer->enabled = 1;
}

void buzzer_trigger(Buzzer *buzzer)
{
    buzzer->enabled ? buzzer_disable(buzzer) : buzzer_enable(buzzer);
}
