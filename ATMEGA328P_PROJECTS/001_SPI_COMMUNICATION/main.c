#include <avr/io.h>

#define F_CPU 16000000UL
#include <util/delay.h>

#include "MAX7219/MAX7219.h"

int main(void) {

    // initialize MAX7219 as SPI on B ports
    // DC (MOSI) -> PB3
    // CS -> PB2
    // CLK -> PB5
    MAX7219_init(&DDRB, &PORTB, &DDRB, &PORTB, 3, 2, 5);

    // shutdown if was previously enabled (in case of MCU reset)
    MAX7219_off();

    // enter test mode
    // in this mode the display is enabled
    MAX7219_enter_test_mode();
    _delay_ms(500);
    MAX7219_exit_test_mode();  // also disables display
    _delay_ms(500);

    // to set medium intensity
    //        scan limit to 8 digits
    //        decode mode to 0x00 (no decode)
    // to clean the display
    // and to enable it!
    MAX7219_set_default_config();
    
    MAX7219_set_decode_mode(0xFF);
    MAX7219_clear();

    int i;

    // testing...
    for (i=1; i<=8; i++) {
        MAX7219_write(i, i-1);
        _delay_ms(500);
    }

    // testing...
    for (i=1; i<=8; i++) {
        MAX7219_write(i, (8-i) | 0b10000000);
        _delay_ms(500);
    }

    // testing... write a "--HELP--" using direct (decode mode) writing
    MAX7219_write(8, 0xA);  // -
    MAX7219_write(7, 0xA);  // -
    MAX7219_write(6, 0xC);  // H
    MAX7219_write(5, 0xB);  // E
    MAX7219_write(4, 0xD);  // L
    MAX7219_write(3, 0xE);  // P
    MAX7219_write(2, 0xA);  // -
    MAX7219_write(1, 0xA);  // -
    _delay_ms(2000);

    MAX7219_set_decode_mode(0);

    // testing...
    MAX7219_print("3.141592");
    _delay_ms(1000);

    // testing...
    MAX7219_print_int32(-7171717L);
    _delay_ms(1000);

    // testing...
    // MAX7219_print_double(-369.13f);
    // _delay_ms(1000);

    // shutdown the device after all testing procedures
    MAX7219_off();
}
