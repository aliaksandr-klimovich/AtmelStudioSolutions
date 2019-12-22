#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


/*
    +--------------------------+
    | LED driver configuration |
    +--------------------------+
*/

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


/*
    +---------------------------------+
    | Delay functionality definitions |
    +---------------------------------+
*/

volatile bool delay50usPassed;

void delay50usConfig()
{
    OCR0A = 99;             // set TOP counter value to (50 us / (1 / (16 MHz / 8))) = 100 (-13)
    TCCR0A = (1 << WGM01);  // CTC mode
    TIMSK0 = (1 << OCIE0A); // enable interrupt
}

void delay50usStart()
{
    sei();
    delay50usPassed = false;
    TCCR0B = (1 << CS01);   // set prescaler to (clk / 8)
}

void delay50usWait()
{
    while(!delay50usPassed) {}
    cli();
}

ISR(TIMER0_COMPA_vect)
{
    TCCR0B = 0;     // stop timer
    TCNT0 = 0;      // reset counter
    delay50usPassed = true;
}


/*
    +------+
    | main |
    +------+
*/

int main(void)
{
    // Disable interrupts if any
    cli();

    // Disable analog comparator (minimize power consumption)
    ACSR = (1 << ACD);

    // Configure timer 0 to make 50 us delay
    delay50usConfig();

    // Setup DDR and PORT on LEDS pin
    LED_DDR |= (1 << LED_PIN);  // pin as output
    LED_PORT &= ~(1 << LED_PIN);  // initially low

    // Prepared test buffer to send to the LEDS input
    buf[0] = (RGB_t){0x55, 0x05, 0xF5};
    buf[1] = (RGB_t){0xAA, 0x0A, 0xFA};

    // Send buffer to the LEDS
    WS2812B_PB0((const RGB_t *) buf, LED_COUNT);
    delay50usStart();

    // Swap 2 colors
    buf[1] = (RGB_t){0x55, 0x05, 0xF5};
    buf[0] = (RGB_t){0xAA, 0x0A, 0xFA};
    delay50usWait();
    WS2812B_PB0((const RGB_t *) buf, LED_COUNT);
    delay50usStart();

    // Swap 2 colors back
    buf[0] = (RGB_t){0x55, 0x05, 0xF5};
    buf[1] = (RGB_t){0xAA, 0x0A, 0xFA};
    delay50usWait();
    WS2812B_PB0((const RGB_t *) buf, LED_COUNT);

    // Sleep forever...
    sleep_enable();
    sleep_cpu();
}
