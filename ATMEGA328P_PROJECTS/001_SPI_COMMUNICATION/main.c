#include <avr/io.h>

#define F_CPU 16000000UL
#include <util/delay.h>

#include "MAX7219/MAX7219.h"
#include <stdlib.h>
#include <string.h>

int main(void) {

    MAX7219_init();

    MAX7219_enter_test_mode();
    _delay_ms(500);
    MAX7219_exit_test_mode();
    _delay_ms(500);

    MAX7219_set_default_config();

    int i;

    for (i=1; i<=8; i++) {
        MAX7219_write(i, i-1);
        _delay_ms(500);
    }

    for (i=1; i<=8; i++) {
        MAX7219_write(i, (8-i) | 0b10000000);
        _delay_ms(500);
    }

    MAX7219_write(8, 0xA);
    MAX7219_write(7, 0xA);
    MAX7219_write(6, 0xC);
    MAX7219_write(5, 0xB);
    MAX7219_write(4, 0xD);
    MAX7219_write(3, 0xE);
    MAX7219_write(2, 0xA);
    MAX7219_write(1, 0xA);
    _delay_ms(1500);

    char str[20];
    i = 255;
    itoa(i, str, 10);
    strcat(str, "\0");
    MAX7219_print(str);
    _delay_ms(1500);

    MAX7219_print("3.141592\0");
    _delay_ms(2500);

    MAX7219_shutdown();

    return 0;
}
