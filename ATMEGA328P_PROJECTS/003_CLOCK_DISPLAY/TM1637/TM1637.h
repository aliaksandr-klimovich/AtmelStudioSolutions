/*
 * TM1637.h
 *
 * Created: 14.06.2019 22:27:54
 *  Author: Aliaksandr
 */

#ifndef TM1637_H_
#define TM1637_H_

#include <avr/sfr_defs.h>

#define TM1637_CMD_DATA_WRITE   0x40
#define TM1637_CMD_INIT_ADDR    0xC0
#define TM1637_CMD_BRIGHTNESS   0x80
#define TM1637_CMD_SCREEN_ON    0x08
#define TM1637_CMD_SCREEN_OFF   0

#define TM1637_BUF_SIZE 4  // 4 is for clock display, max is 8
#define TM1637_DIGITS_SIZE 16

uint8_t TM1637_delay;
uint8_t TM1637_brightness;
uint8_t TM1637_screen_on;

const uint8_t TM1637_DIGITS[TM1637_DIGITS_SIZE];
uint8_t TM1637_buf[TM1637_BUF_SIZE];

void TM1637_init(volatile uint8_t *clk_ddr, volatile uint8_t *clk_port_reg, uint8_t clk_port_num,
                 volatile uint8_t *dio_ddr, volatile uint8_t *dio_port_reg, volatile uint8_t *dio_pin_reg, uint8_t dio_port_num);

void TM1637_write_SRAM_auto_increment(uint8_t cmd1, uint8_t cmd2, uint8_t cmd3,
                                      uint8_t data[], uint8_t len);

void TM1637_write_buffer(void);                                      

#endif /* TM1637_H_ */
