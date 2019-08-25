#include <avr/io.h>

#include "buzzer.h"
#include "../main.h"

void buzzer0_init()
{
    DDR_SET_P(buzzer0.dio);
    PORTR_SET_P(buzzer0.dio);

    buzzer0_disable();

    // Set clear timer on compare match (CTC) mode
    TCCR2A = (1 << WGM21);

    // Configure tone
    OCR2A = 0x1F;  // Output compare match register
    TCCR2B = (1 << CS22);  // CLK/128 (from prescaler)
}

void buzzer0_disable()
{
    // Disconnect output compare unit
    TCCR2A &= ~(1 << COM2B0);
    buzzer0.enabled = false;
}

void buzzer0_enable()
{
    // Toggle OC2A on compare match
    TCCR2A |= (1 << COM2B0);
    buzzer0.enabled = true;
}

void buzzer0_trigger()
{
    buzzer0.enabled ? buzzer0_disable() : buzzer0_enable();
}
