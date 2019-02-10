#include "MAX7219.h"

typedef struct {
    char character;
    uint8_t code;
} _MAX7219_table;

static const _MAX7219_table MAX7219_table[] = {
    {'0', 0b01111110},  // O, D
    {'1', 0b00110000},
    {'2', 0b01101101},  // Z
    {'3', 0b01111001},
    {'4', 0b00110011},  // Y
    {'5', 0b01011011},  // S
    {'6', 0b01011111},  // G
    {'7', 0b01110000},
    {'8', 0b01111111},  // B
    {'9', 0b01111011},
    {'-', 0b00000001},
    {' ', 0b00000000},
    {'H', 0b00110111},  // X, K
    {'E', 0b01001111},
    {'L', 0b00001110},
    {'P', 0b01100111},
    {'_', 0b00001000},
    {'U', 0b00111110},
    {'F', 0b01000111},
    {'C', 0b01001110},
    {'d', 0b00111101},
    {'b', 0b00011111},
    {'A', 0b01110111},  // R
    {'J', 0b00111100},
    {'*', 0b01100011},  // degree sign
};

static const uint8_t MAX7219_TABLE_SIZE = sizeof(MAX7219_table) / sizeof(MAX7219_table[0]);

volatile uint8_t * MAX7219_DDR;
volatile uint8_t * MAX7219_PORT;
volatile uint8_t * MAX7219_CS_DDR;
volatile uint8_t * MAX7219_CS_PORT;
uint8_t MAX7219_DIN, MAX7219_CS, MAX7219_CLK;

/*
    Initialization function
    SPI interface should be located at the same port (for example, B)
    CS port can be different 
*/
void MAX7219_init(volatile uint8_t * DDR_ADDR, volatile uint8_t * PORT_ADDR, 
                  volatile uint8_t * CS_DDR_ADDR, volatile uint8_t * CS_PORT_ADDR,
                  uint8_t DIN, uint8_t CS, uint8_t CLK) {                 
    
    MAX7219_DDR = DDR_ADDR;
    MAX7219_PORT = PORT_ADDR;
    MAX7219_CS_DDR = CS_DDR_ADDR;
    MAX7219_CS_PORT = CS_PORT_ADDR;
    MAX7219_DIN = DIN;
    MAX7219_CS = CS;
    MAX7219_CLK = CLK;

    *MAX7219_DDR |= (1 << MAX7219_DIN) | (1 << MAX7219_CLK);
    *MAX7219_CS_DDR |= (1 << MAX7219_CS);
    // it is not necessary to set MOSI and CLK pins as output
    // as they functionality will be overridden by the HW 
    *MAX7219_CS_PORT |= (1 << MAX7219_CS);   
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void MAX7219_write(uint8_t addr, uint8_t data) {
    *MAX7219_CS_PORT &= ~(1 << MAX7219_CS);
    SPDR = addr;
    while(!(SPSR & (1 << SPIF)));
    SPDR = data;
    while(!(SPSR & (1 << SPIF)));
    *MAX7219_CS_PORT |= (1 << MAX7219_CS);
}

void MAX7219_on(void) {
    MAX7219_write(0x0C, 1);
}

void MAX7219_off(void) {
    MAX7219_write(0x0C, 0);
}

void MAX7219_enter_test_mode(void) {
    MAX7219_write(0x0F, 1);
    // cannot leave test mode until `exit_test_mode` is executed
}

void MAX7219_exit_test_mode(void) {
    MAX7219_write(0x0F, 0);
}

void MAX7219_set_scan_limit(uint8_t sl /* 0 - 7 */) {
#if MAX7219_PARAM_CHECKS == 1
    if (sl >= 0 && sl <= 7) {
#endif
        MAX7219_write(0x0B, sl);
#if MAX7219_PARAM_CHECKS == 1
    }
#endif
}

void MAX7219_set_decode_mode(uint8_t dm /* 0x00 - 0xFF */) {
    MAX7219_write(0x09, dm);
    // do not forget to clean the screen
}

void MAX7219_clear(void) {
    for (uint8_t i=1; i<=8; ++i) {
        MAX7219_write(i, 0x00);  // no segment is powered in no decode mode
    }
}

void MAX7219_set_intensity(uint8_t intensity /* 0x00 - 0x0F */) {
#if MAX7219_PARAM_CHECKS == 1
    if (intensity >= 0 && intensity <= 0x0F) {
#endif
        MAX7219_write(0x0A, intensity);
#if MAX7219_PARAM_CHECKS == 1
    }
#endif
}

void MAX7219_set_default_config(void) {
    MAX7219_set_intensity(7);
    MAX7219_set_scan_limit(7);
    MAX7219_set_decode_mode(0x00);
    MAX7219_clear();
    MAX7219_on();
}

void MAX7219_print(char* string) {  // string should be \0 terminated (ASCIIZ string)

    uint8_t buf[8];
    uint8_t index_buf = 0;
    uint8_t index_string = 0;

    while(index_buf < 8 && string[index_string] != '\0') {

        if (string[index_string] == '.') {

            if (index_buf == 0) {
                buf[index_buf] = 0x80;
            } else {
                buf[--index_buf] |= 0x80;
            }

        } else {

            for (uint8_t i=0; i<MAX7219_TABLE_SIZE; ++i) {
                if (MAX7219_table[i].character == string[index_string]) {
                    buf[index_buf] = MAX7219_table[i].code;
                    break;
                }
            }
        }

        ++index_buf;
        ++index_string;
    }

    if (string[index_string] == '.') {
        buf[7] |= 0x80;
    }

    // use index_string as index to write only the buffer to the display
    // index_buf contains the buffer size
    // MAX7219_write should accept the digit number and index_buf already contains +1
    for (index_string=0; index_string<index_buf; ++index_string) {
        MAX7219_write((index_buf - index_string), buf[index_string]);
    }

    // fill the rest of the screen with empty
    // index_buf still contains the buffer size
    // index_buf moved to the next element of the buffer if it is present int eh buffer   
    for (++index_buf; index_buf<=8; ++index_buf) {
        MAX7219_write(index_buf, 0);
    }
}

/*
    Convert integer to ASCIIZ.
    `buf` size should be equal to `width` in case of positive value
        and `width + 1` is case of negative value.  
*/ 
/* static void int_to_str (int32_t val, uint8_t width, char * buf) {
    uint8_t i = 0;
    uint8_t j = 0;  // stop index
    
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
    } else {
        if (val < 0) {
            buf[0] = '-';
            val = -val;
            ++j;
        }
        i = width;
        while (val && i > j) {
            buf[--i] = val % 10 + '0';
            val /= 10;
        }
        if (i != j) {
            for (; i < width; j++, i++) {
                buf[j] = buf[i];
            }
            buf[j] = '\0';
        } else {
            buf[width] = '\0';
        }
    }
} */

void MAX7219_print_int(int32_t val) {
#if MAX7219_PARAM_CHECKS == 1
    if (val >= -9999999 && val <= 99999999) {
#endif
        char buf[9];
        ltoa(val, buf, 10);
        MAX7219_print(buf);
#if MAX7219_PARAM_CHECKS == 1
    }
#endif    
}

void MAX7219_print_float(float value, uint8_t prec /* nums count after point */) { 
    char buf[10];
    dtostrf(value, 8, prec, buf);
    MAX7219_print(buf);
}
