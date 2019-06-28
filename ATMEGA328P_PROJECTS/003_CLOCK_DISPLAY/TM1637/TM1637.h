/*
 * TM1637.h
 *
 * Created: 14.06.2019 22:27:54
 *  Author: Aliaksandr
 */

#ifndef TM1637_H_
#define TM1637_H_

#include <avr/sfr_defs.h>

void TM1637_init(volatile uint8_t *clk_ddr, volatile uint8_t *clk_port_reg, uint8_t clk_port_num,
                 volatile uint8_t *dio_ddr, volatile uint8_t *dio_port_reg, uint8_t dio_port_num);


#endif /* TM1637_H_ */
