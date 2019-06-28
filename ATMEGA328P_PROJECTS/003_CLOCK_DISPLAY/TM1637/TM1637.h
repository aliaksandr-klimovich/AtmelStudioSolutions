/*
 * TM1637.h
 *
 * Created: 14.06.2019 22:27:54
 *  Author: Aliaksandr
 */

#ifndef TM1637_H_
#define TM1637_H_

#include <avr/sfr_defs.h>

#define TM1637_CMD_DATA_WRITE 0x40
#define TM1637_CMD_INIT_ADDR 0xC0
#define TM1637_CMD_BRIGHTNESS 0x80

void TM1637_init(volatile uint8_t *clk_ddr, volatile uint8_t *clk_port_reg, uint8_t clk_port_num,
                 volatile uint8_t *dio_ddr, volatile uint8_t *dio_port_reg, volatile uint8_t *dio_pin_reg, uint8_t dio_port_num, 
                 uint8_t cmd_delay);

void TM1637_write_SRAM_auto_increment(uint8_t cmd1, uint8_t cmd2, uint8_t cmd3,
                                      uint8_t data[], uint8_t len);

#endif /* TM1637_H_ */
