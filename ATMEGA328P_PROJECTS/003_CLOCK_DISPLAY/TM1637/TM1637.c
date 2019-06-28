/*
 * TM1637.c
 *
 * Created: 14.06.2019 22:27:45
 *  Author: Aliaksandr
 */

#include "TM1637.h"

volatile uint8_t *TM1637_CLK_DDR;
volatile uint8_t *TM1637_CLK_PORT_REG;
uint8_t TM1637_CLK_PORT_NUM;

volatile uint8_t *TM1637_DIO_DDR;
volatile uint8_t *TM1637_DIO_PORT_REG;
uint8_t TM1637_DIO_PORT_NUM;

void TM1637_init(volatile uint8_t *clk_ddr, volatile uint8_t *clk_port_reg, uint8_t clk_port_num,
                 volatile uint8_t *dio_ddr, volatile uint8_t *dio_port_reg, uint8_t dio_port_num)
{
    // store CLK pin configuration
    TM1637_CLK_DDR = clk_ddr;
    TM1637_CLK_PORT_REG = clk_port_reg;
    TM1637_CLK_PORT_NUM = clk_port_num;

    // set CLK pin as a high output
    *TM1637_CLK_DDR |= (1 << TM1637_CLK_PORT_NUM);
    *TM1637_CLK_PORT_REG |= (1 << TM1637_CLK_PORT_NUM);

    // store DIO pin configuration
    TM1637_DIO_DDR = dio_ddr;
    TM1637_DIO_PORT_REG = dio_port_reg;
    TM1637_DIO_PORT_NUM = dio_port_num;

    // set DIO pin as a high output
    *TM1637_DIO_DDR |= (1 << TM1637_DIO_PORT_NUM);
    *TM1637_DIO_PORT_REG |= (1 << TM1637_DIO_PORT_NUM);
}


