#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "WS2812B.h"

// Define LED strip (LEDS) pin
#define LED_DDR     DDRB
#define LED_PORT    PORTB
#define LED_PIN     PB0

// and the LED-s count in the strip
#define LED_COUNT   2U

// Create RGB buffer
RGB_t buf[LED_COUNT];

// Create functions to associate it with LEDS pin
WS2812B_CREATE_FUNCTION(WS2812B_PB0, LED_PORT, LED_PIN);

int main(void)
{
    // Disable interrupts if any
    cli();

    // Disable analog comparator (minimize power consumption)
    ACSR = (1 << ACD);

    // Setup DDR and PORT on LEDS pin
    LED_DDR |= (1 << LED_PIN);  // pin as output
    LED_PORT &= ~(1 << LED_PIN);  // initially low

    // Prepared test buffer to send to the LEDS input
    buf[0] = (RGB_t){0x55, 0x05, 0xF5};
    buf[1] = (RGB_t){0xAA, 0x0A, 0xFA};

    // Send buffer to the LEDS
    WS2812B_PB0((const RGB_t *) buf, LED_COUNT);

    // Sleep forever...
    sleep_enable();
    while (1)
    {
        sleep_cpu();
    }
}
