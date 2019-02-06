#ifndef MAX7219_H_
#define MAX7219_H_

#include <avr/io.h>
#include <stdint.h>

#define MAX7219_DDR     DDRB
#define MAX7219_PORT    PORTB
#define MAX7219_PIN     PINB

#define MAX7219_DIN     3  // MOSI
#define MAX7219_CS      2  // SS
#define MAX7219_CLK     5  // SCK 

void MAX7219_init(void);
void MAX7219_write(uint8_t addr, uint8_t data);
void MAX7219_shutdown(void);
void MAX7219_normal_operation(void);
void MAX7219_enter_test_mode(void);
void MAX7219_exit_test_mode(void);
void MAX7219_set_scan_limit(uint8_t sl);
void MAX7219_set_decode_mode(uint8_t dm);
void MAX7219_set_no_decode_mode(void);
void MAX7219_clean(void);
void MAX7219_set_intensity(uint8_t intensity);
void MAX7219_set_default_config(void);
void MAX7219_print(char* s);

#endif /* MAX7219_H_ */