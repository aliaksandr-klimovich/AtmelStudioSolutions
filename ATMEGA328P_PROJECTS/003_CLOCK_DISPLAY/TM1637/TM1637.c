#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "TM1637.h"

#define TM1637_CMD_DATA_WRITE   0b01000000
#define TM1637_CMD_INIT_ADDR    0b11000000
#define TM1637_CMD_BRIGHTNESS   0b10000000
#define TM1637_CMD_SCREEN_ON    0b00001000
#define TM1637_CMD_SCREEN_OFF   0

#define TM1637_DELAY  0

const uint8_t TM1637_CHAR_TABLE[] = {
   // XGFEDCBA (X is DP)
    0b00111111,  // 0
    0b00000110,  // 1
    0b01011011,  // 2
    0b01001111,  // 3
    0b01100110,  // 4
    0b01101101,  // 5
    0b01111101,  // 6
    0b00000111,  // 7
    0b01111111,  // 8
    0b01101111,  // 9
    0b01110111,  // A = 10
    0b01111100,  // b = 11
    0b00111001,  // C = 12
    0b01011110,  // d = 13
    0b01111001,  // E = 14
    0b01110001,  // F = 15
};

void TM1637_init(TM1637_driver *driver)
{
    driver->buf = (uint8_t *)malloc(driver->buf_size * sizeof(uint8_t));  // check for null is absent!

    // Set CLK pin as a high output
    PORTR_SET(driver->clk);  // CLK to HIGH
    DDR_SET(driver->clk);    // CLK as output

    // Set DIO pin as a high output
    PORTR_SET(driver->dio);  // DIO to HIGH
    DDR_SET(driver->dio);    // DIO as output
}

static inline void TM1637_cmd_delay()
{
    // Make a light delay between send commands for very fast transmission
    for (uint16_t i=0; i < TM1637_DELAY; i++)
    {
        asm("NOP");
    }
}

static void TM1637_write_byte(TM1637_driver *driver, uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        PORTR_CLEAR(driver->clk);  // CLK to LOW
        TM1637_cmd_delay();

        if (data & 1)
            PORTR_SET(driver->dio);  // DIO to HIGH
        else
            PORTR_CLEAR(driver->dio);  // DIO to LOW
        TM1637_cmd_delay();

        PORTR_SET(driver->clk);  // CLK to HIGH
        TM1637_cmd_delay();

        data >>= 1;
    }
}

static void TM1637_start(TM1637_driver *driver)
{
    // Generate start condition
    PORTR_CLEAR(driver->dio);  // DIO to LOW
    TM1637_cmd_delay();
}

static void TM1637_stop(TM1637_driver *driver)
{
    // Generate stop condition
    PORTR_SET(driver->clk); // CLK to HIGH
    TM1637_cmd_delay();
    PORTR_SET(driver->dio); // DIO to HIGH
    TM1637_cmd_delay();
}

static uint8_t TM1637_read_ack(TM1637_driver *driver)
{
    uint8_t ack;

    // Read ACK from MCU
    PORTR_CLEAR(driver->clk);  // CLK to LOW
    PORTR_CLEAR(driver->dio);  // DIO to LOW, intermediate state, consult atmega328p ref.man.
    DDR_CLEAR(driver->dio);    // DIO as input
    TM1637_cmd_delay();

    PORTR_SET(driver->clk);  // CLK to HIGH
    TM1637_cmd_delay();

    // Read DIO, should be 0 if ACK is received
    ack = PINR_READ(driver->dio);
    if (ack != 0)
    {
        // TODO: error handling if ACK was not received
    }

    PORTR_CLEAR(driver->clk);  // CLK to LOW
    DDR_SET(driver->dio);      // DIO as output (already low)
    TM1637_cmd_delay();

    return ack;
}

static void TM1637_write_SRAM_auto_increment(TM1637_driver *driver,
                                             uint8_t cmd1, uint8_t cmd2, uint8_t cmd3,
                                             uint8_t *data, uint8_t len)
{
    TM1637_start(driver);
    TM1637_write_byte(driver, cmd1);
    TM1637_read_ack(driver);
    TM1637_stop(driver);

    TM1637_start(driver);
    TM1637_write_byte(driver, cmd2);
    TM1637_read_ack(driver);
    for (uint8_t i = 0; i < len; i++)
    {
        TM1637_write_byte(driver, data[i]);
        TM1637_read_ack(driver);
    }
    TM1637_stop(driver);

    TM1637_start(driver);
    TM1637_write_byte(driver, cmd3);
    TM1637_read_ack(driver);
    TM1637_stop(driver);
}

void TM1637_send_buffer(TM1637_driver *driver)
{
    TM1637_write_SRAM_auto_increment
    (
        driver,
        TM1637_CMD_DATA_WRITE,
        TM1637_CMD_INIT_ADDR,
        TM1637_CMD_BRIGHTNESS |
        (driver->brightness & 0x07) |
        (driver->screen_on ? TM1637_CMD_SCREEN_ON : TM1637_CMD_SCREEN_OFF),
        driver->buf,
        driver->buf_size
    );
}

void TM1637_send_uint16(TM1637_driver *driver, uint16_t value)
{
    for ( uint8_t i = 0; i < driver->buf_size; i++ )
    {
        driver->buf[driver->buf_size - i - 1] = TM1637_CHAR_TABLE[value % 10];
        value /= 10;
    }
    TM1637_send_buffer(driver);
}
