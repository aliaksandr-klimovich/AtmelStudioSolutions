#ifndef MAX7219_H_
#define MAX7219_H_

#include <avr/io.h>
#include <stdint.h>

void MAX7219_init(volatile uint8_t * DDR, volatile uint8_t * PORT, uint8_t DIN, uint8_t CS, uint8_t CLK);
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