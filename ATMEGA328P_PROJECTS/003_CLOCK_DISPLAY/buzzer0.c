#include "buzzer0.h"

void buzzer0_configure()
{
    DDR_SET(PD3);
    PORT_SET(PD3);

    buzzer0_disable();

    // Configure tone
    OCR2B = 0x10;  // Output compare match register
    TCCR2B = (1 << CS21);  // clkI/O/8 (from prescaler)
}

void buzzer0_disable()
{
    TCCR2A = 0;
}

void buzzer0_enable()
{
    // Toggle OC2A on compare match
    // Set clear timer on compare match (CTC) mode
    TCCR2A = (1 << COM2B0) | (1 << WGM21);
}
