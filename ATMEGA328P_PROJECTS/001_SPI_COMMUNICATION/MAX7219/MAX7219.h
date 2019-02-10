/*
    MAX7219 library
    version 1.2.0
    
    Limitations:
        - no decode mode is used (setting decode mode will lead to improper functioning)
        - supports only 8 digits   
*/

#ifndef MAX7219_H_
#define MAX7219_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>

/* 
    check input parameters for correctness?
    if 0 will use less memory and runtime
*/
#define MAX7219_PARAM_CHECKS 0

void MAX7219_init(volatile uint8_t * DDR_ADDR, 
                  volatile uint8_t * PORT_ADDR,
                  volatile uint8_t * CS_DDR_ADDR,
                  volatile uint8_t * CS_PORT_ADDR, 
                  uint8_t DIN, uint8_t CS, uint8_t CLK);
void MAX7219_write(uint8_t address, uint8_t data);  // low level function (LLF)
void MAX7219_off(void);
void MAX7219_on(void);
void MAX7219_enter_test_mode(void);
void MAX7219_exit_test_mode(void);
void MAX7219_set_scan_limit(uint8_t scan_limit);  // LLF
void MAX7219_set_decode_mode(uint8_t decode_mode);  // LLF
void MAX7219_clear(void);  
void MAX7219_set_intensity(uint8_t intensity);  
void MAX7219_set_default_config(void);
void MAX7219_print(char* s);
void MAX7219_print_int(int32_t value);
void MAX7219_print_float(float value, uint8_t precision);

#endif /* MAX7219_H_ */