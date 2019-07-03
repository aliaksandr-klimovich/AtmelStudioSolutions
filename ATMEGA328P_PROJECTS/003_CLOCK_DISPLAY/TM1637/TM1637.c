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

uint8_t TM1637_buf[TM1637_BUF_SIZE] = {0};
uint8_t TM1637_brightness = 0;  // 0 .. 7
uint8_t TM1637_screen_on = 0;   // 0 (on) .. 1 (off)

const uint8_t TM1637_CHAR_TABLE[TM1637_CHAR_TABLE_SIZE] = {
   // XGFEDCBA   (X is dp)
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
    0b01110111,  // A
    0b01111100,  // b
    0b00111001,  // C
    0b01011110,  // d
    0b01111001,  // E
    0b01110001   // F
};

void TM1637_init(volatile uint8_t *clk_ddr, volatile uint8_t *clk_port_reg, uint8_t clk_port_num,
                 volatile uint8_t *dio_ddr, volatile uint8_t *dio_port_reg, volatile uint8_t *dio_pin_reg,
                 uint8_t dio_port_num)
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
}

static void TM1637_cmd_delay()
{
    for (uint16_t i=0; i < TM1637_DELAY; i++)
    {
        asm("nop");
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

static void TM1637_write_SRAM_auto_increment(uint8_t cmd1, uint8_t cmd2, uint8_t cmd3,
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

inline void TM1637_write_buffer(void)
{
    TM1637_write_SRAM_auto_increment
    (
        TM1637_CMD_DATA_WRITE,
        TM1637_CMD_INIT_ADDR,
        TM1637_CMD_BRIGHTNESS |
            (TM1637_brightness & 0x07) |
            (TM1637_screen_on ? TM1637_CMD_SCREEN_ON : TM1637_CMD_SCREEN_OFF),
        TM1637_buf,
        TM1637_BUF_SIZE
    );
}

void TM1637_print(const char * s, ...)
{
    char buf[TM1637_BUF_SIZE + 2] = {0};  // 2 = ':' + NUL terminator
    char *p_buf = buf;  // pointer to buf
    char c;  // char from buf
    uint8_t i;  // buf index
    uint8_t j;  // TM1637_buf index

    va_list ap;
    va_start(ap, s);
    vsnprintf(p_buf, (size_t)(TM1637_BUF_SIZE + 2), s, ap);
    va_end(ap);

    memset(TM1637_buf, 0, TM1637_BUF_SIZE);

    for (i=0, j=0; /* infinite loop */; i++, j++)
    {
        c = buf[i];

        if (c == 0)  // buffer is null terminated
            break;
        else if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'a' && c <= 'f')
        {
            c -= 'a';
            c += 10;  // to properly address the TM1637_CHAR_TABLE table
        }
        else if (c >= 'A' && c <= 'F')
        {
            c -= 'A';
            c += 10;
        }
        else if ((c == ':' || c == '.') && i == 2)
        {
            TM1637_buf[--j] |= 0x80;  // add "dp" to previous character
            continue;
        }
        else if ((c == ' ' || c == '_') && i == 2)
        {
            j--;
            continue;
        }
        else
            continue;

        TM1637_buf[j] = TM1637_CHAR_TABLE[(uint8_t)c];
    }

    TM1637_write_buffer();
}
