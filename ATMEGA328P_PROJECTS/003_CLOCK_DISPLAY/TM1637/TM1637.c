#include "TM1637.h"

PIN *TM1637_CLK;
PIN *TM1637_DIO;

uint8_t TM1637_buf[TM1637_BUF_SIZE] = {0};
uint8_t TM1637_brightness = 0;  // 0 .. 7
uint8_t TM1637_screen_on = 1;   // 0 (off) .. 1 (on),
// First buffer_write command will switch on the screen if 1 (on) is set

const uint8_t TM1637_CHAR_TABLE[TM1637_CHAR_TABLE_SIZE] = {
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
    0b01110111,  // A
    0b01111100,  // b
    0b00111001,  // C
    0b01011110,  // d
    0b01111001,  // E
    0b01110001   // F
};

void TM1637_init(PIN * const clk, PIN * const dio)
{
    TM1637_CLK = clk;  // Store CLK pin
    TM1637_DIO = dio;  // Store DIO pin

    // Set CLK pin as a high output
    PORTR_SET_P(TM1637_CLK);  // CLK to HIGH
    DDR_SET_P(TM1637_CLK);  // CLK as output

    // Set DIO pin as a high output
    PORTR_SET_P(TM1637_DIO);  // DIO to HIGH
    DDR_SET_P(TM1637_DIO);   // DIO as output
}

static void TM1637_cmd_delay()
{
    for (uint16_t i=0; i < TM1637_DELAY; i++)
    {
        asm("NOP");
    }
}

static void TM1637_write_byte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        PORTR_CLEAR_P(TM1637_CLK);  // CLK to LOW
        TM1637_cmd_delay();

        if (data & 1)
        {
            PORTR_SET_P(TM1637_DIO);  // DIO to HIGH
        }
        else
        {
            PORTR_CLEAR_P(TM1637_DIO);  // DIO to LOW
        }
        TM1637_cmd_delay();

        PORTR_SET_P(TM1637_CLK);  // CLK to HIGH
        TM1637_cmd_delay();

        data >>= 1;
    }
}

static void TM1637_start()
{
    // Generate start condition
    PORTR_CLEAR_P(TM1637_DIO);  // DIO to LOW
    TM1637_cmd_delay();
}

static void TM1637_stop()
{
    // Generate stop condition
    PORTR_SET_P(TM1637_CLK); // CLK to HIGH
    TM1637_cmd_delay();
    PORTR_SET_P(TM1637_DIO); // DIO to HIGH
    TM1637_cmd_delay();
}

static uint8_t TM1637_read_ack()
{
    uint8_t ack;

    // Read ACK from MCU
    PORTR_CLEAR_P(TM1637_CLK);  // CLK to LOW
    PORTR_CLEAR_P(TM1637_DIO);  // DIO to LOW, intermediate state, consult atmega328p ref.man.
    DDR_CLEAR_P(TM1637_DIO);   // DIO as input
    TM1637_cmd_delay();

    PORTR_SET_P(TM1637_CLK);  // CLK to HIGH
    TM1637_cmd_delay();

    // Read DIO, should be 0 if ACK is received
    ack = PINR_READ_P(TM1637_DIO);
    if (ack != 0)
    {
        // TODO: do something if ACK was not received
    }

    PORTR_CLEAR_P(TM1637_CLK);  // CLK to LOW
    DDR_SET_P(TM1637_DIO);   // DIO as output (already low)
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

inline void TM1637_send_buffer(void)
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
        {
            break;
        }
        else if (c >= '0' && c <= '9')
        {
            c -= '0';
        }
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
            TM1637_buf[--j] |= 0x80;  // add DP to previous character
            continue;
        }
        else if ((c == ' ' || c == '_') && i == 2)
        {
            j--;
            continue;
        }
        else
        {
            continue;
        }

        TM1637_buf[j] = TM1637_CHAR_TABLE[(uint8_t)c];
    }

    TM1637_send_buffer();
}
