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
volatile uint8_t *TM1637_DIO_PIN_REG;
uint8_t TM1637_DIO_PORT_NUM;

static uint8_t TM1637_CMD_DELAY;

void TM1637_init(volatile uint8_t *clk_ddr, volatile uint8_t *clk_port_reg, uint8_t clk_port_num,
                 volatile uint8_t *dio_ddr, volatile uint8_t *dio_port_reg, volatile uint8_t *dio_pin_reg, uint8_t dio_port_num,

                 uint8_t cmd_delay)
{
    // store CLK pin configuration
    TM1637_CLK_DDR = clk_ddr;
    TM1637_CLK_PORT_REG = clk_port_reg;
    TM1637_CLK_PORT_NUM = clk_port_num;

    // set CLK pin as a high output
    *TM1637_CLK_PORT_REG |= (1 << TM1637_CLK_PORT_NUM);  // CLK to HIGH
    *TM1637_CLK_DDR |= (1 << TM1637_CLK_PORT_NUM);  // CLK as output

    // store DIO pin configuration
    TM1637_DIO_DDR = dio_ddr;
    TM1637_DIO_PORT_REG = dio_port_reg;
    TM1637_DIO_PIN_REG = dio_pin_reg;
    TM1637_DIO_PORT_NUM = dio_port_num;

    // set DIO pin as a high output
    *TM1637_DIO_PORT_REG |= (1 << TM1637_DIO_PORT_NUM);  // DIO to HIGH
    *TM1637_DIO_DDR |= (1 << TM1637_DIO_PORT_NUM);  // DIO as output

    TM1637_CMD_DELAY = cmd_delay;
}

static void TM1637_cmd_delay()
{
    for (uint8_t i = 0; i < TM1637_CMD_DELAY; i++)
    {
        asm("NOP\n");
    }
}

static void TM1637_write_byte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        *TM1637_CLK_PORT_REG &= ~(1 << TM1637_CLK_PORT_NUM);  // CLK to LOW
        TM1637_cmd_delay();

        if (data & 1)
            *TM1637_DIO_PORT_REG |= (1 << TM1637_DIO_PORT_NUM);  // DIO to HIGH
        else
            *TM1637_DIO_PORT_REG &= ~(1 << TM1637_DIO_PORT_NUM);  // DIO to LOW
        TM1637_cmd_delay();

        *TM1637_CLK_PORT_REG |= (1 << TM1637_CLK_PORT_NUM);  // CLK to HIGH
        TM1637_cmd_delay();

        data >>= 1;
    }
}

static void TM1637_start()
{
    // generate start condition
    *TM1637_DIO_PORT_REG &= ~(1 << TM1637_DIO_PORT_NUM);  // DIO to LOW
    TM1637_cmd_delay();
}

static void TM1637_stop()
{
    // generate stop condition
    *TM1637_CLK_PORT_REG |= (1 << TM1637_CLK_PORT_NUM);  // CLK to HIGH
    TM1637_cmd_delay();
    *TM1637_DIO_PORT_REG |= (1 << TM1637_DIO_PORT_NUM);  // DIO to HIGH
    TM1637_cmd_delay();
}

static uint8_t TM1637_read_ack()
{
    uint8_t ack;

    // read ACK from MCU
    *TM1637_CLK_PORT_REG &= ~(1 << TM1637_CLK_PORT_NUM);  // CLK to LOW
    *TM1637_DIO_PORT_REG &= ~(1 << TM1637_DIO_PORT_NUM);  // DIO to LOW, intermediate state, consult atmega328p ref.man.
    *TM1637_DIO_DDR &= ~(1 << TM1637_DIO_PORT_NUM);  // DIO as input
    TM1637_cmd_delay();

    *TM1637_CLK_PORT_REG |= (1 << TM1637_CLK_PORT_NUM);  // CLK to HIGH
    TM1637_cmd_delay();

    // read DIO, should be 0 in fault free case
    ack = (*TM1637_DIO_PIN_REG & (1 << TM1637_DIO_PORT_NUM)) == 0 ? 0 : 1;

    *TM1637_CLK_PORT_REG &= ~(1 << TM1637_CLK_PORT_NUM);  // CLK to LOW
    *TM1637_DIO_DDR |= (1 << TM1637_DIO_PORT_NUM);  // DIO as output (already low)
    TM1637_cmd_delay();

    return ack;
}

void TM1637_write_SRAM_auto_increment(uint8_t cmd1, uint8_t cmd2, uint8_t cmd3,
                                      uint8_t data[], uint8_t len)
{
    TM1637_start();
    TM1637_write_byte(cmd1);
    TM1637_read_ack();
    TM1637_stop();

    TM1637_start();
    TM1637_write_byte(cmd2);
    TM1637_read_ack();
    for (uint8_t i = 0; i < len; i++)
    {
        TM1637_write_byte(data[i]);
        TM1637_read_ack();
    }
    TM1637_stop();

    TM1637_start();
    TM1637_write_byte(cmd3);
    TM1637_read_ack();
    TM1637_stop();
}
