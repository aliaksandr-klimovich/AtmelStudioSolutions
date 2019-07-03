/*
 * TM1637.h
 *
 * Created: 14.06.2019 22:27:54
 *  Author: Aliaksandr
 */

#ifndef TM1637_H_
#define TM1637_H_


#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <avr/sfr_defs.h>


#define TM1637_CMD_DATA_WRITE   0b01000000
#define TM1637_CMD_INIT_ADDR    0b11000000
#define TM1637_CMD_BRIGHTNESS   0b10000000
#define TM1637_CMD_SCREEN_ON    0b00001000
#define TM1637_CMD_SCREEN_OFF   0

#define TM1637_DELAY 100

#define TM1637_BUF_SIZE 4  // 4 stands here for clock display, max is 8 for this MCU
uint8_t TM1637_buf[TM1637_BUF_SIZE];

#define TM1637_CHAR_TABLE_SIZE 16
const uint8_t TM1637_CHAR_TABLE[TM1637_CHAR_TABLE_SIZE];

uint8_t TM1637_brightness;
uint8_t TM1637_screen_on;

void TM1637_init(volatile uint8_t *clk_ddr, volatile uint8_t *clk_port_reg, uint8_t clk_port_num,
                 volatile uint8_t *dio_ddr, volatile uint8_t *dio_port_reg, volatile uint8_t *dio_pin_reg,
                 uint8_t dio_port_num);

void TM1637_write_buffer(void);
void TM1637_print(const char * s, ...);


#endif /* TM1637_H_ */
