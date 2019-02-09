#include "MAX7219.h"

static uint8_t MAX7219_decode_mode = 0;

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
};
static const size_t MAX7219_TABLE_SIZE = sizeof(MAX7219_table) / sizeof(MAX7219_table[0]);

volatile uint8_t * MAX7219_DDR;
volatile uint8_t * MAX7219_PORT;
uint8_t MAX7219_DIN, MAX7219_CS, MAX7219_CLK;

void MAX7219_init(volatile uint8_t * DDR_ADDR, volatile uint8_t * PORT_ADDR, uint8_t DIN, uint8_t CS, uint8_t CLK) {
    MAX7219_DDR = DDR_ADDR;
    MAX7219_PORT = PORT_ADDR;
    MAX7219_DIN = DIN;
    MAX7219_CS = CS;
    MAX7219_CLK = CLK;

    *MAX7219_DDR |= (1 << MAX7219_DIN) | (1 << MAX7219_CS) | (1 << MAX7219_CLK);
    *MAX7219_PORT |= (1 << MAX7219_CS);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

    MAX7219_DOUBLE_PREC = 2;
}

void MAX7219_write(uint8_t addr, uint8_t data) {
    *MAX7219_PORT &= ~(1 << MAX7219_CS);
    SPDR = addr;
    while(!(SPSR & (1 << SPIF)));
    SPDR = data;
    while(!(SPSR & (1 << SPIF)));
    *MAX7219_PORT |= (1 << MAX7219_CS);
}

void MAX7219_set_digit(uint8_t digit_number, uint8_t value) {
    if (digit_number >= 1 && digit_number <= 8 && value >= 0 && value <= 9) {
        int addr = 8 - digit_number;
        if (MAX7219_decode_mode & (1 << addr)) {
            MAX7219_write(addr, value); 
        } else {
            MAX7219_write(addr, MAX7219_table[value].code);            
        }         
    }
}

void MAX7219_shutdown(void) {
    MAX7219_write(0x0C, 0);
}

void MAX7219_normal_operation(void) {
    MAX7219_write(0x0C, 1);
}

void MAX7219_enter_test_mode(void) {
    MAX7219_write(0x0F, 1);
    // cannot leave test mode until `exit_test_mode` is executed
}

void MAX7219_exit_test_mode(void) {
    MAX7219_write(0x0F, 0);
}

void MAX7219_set_scan_limit(uint8_t sl /* 0 - 7 */) {
    if (sl >= 0 && sl <= 7) {
        MAX7219_write(0x0B, sl);
    }
}

void MAX7219_set_decode_mode(uint8_t dm /* 0x00 - 0xFF */) {
    MAX7219_write(0x09, dm);
    MAX7219_decode_mode = dm;
    // do not forget to clean the screen
}

void MAX7219_set_no_decode_mode(void) {
    MAX7219_write(0x09, 0x00);
    MAX7219_decode_mode = 0;
    // do not forget to clean the screen
}

void MAX7219_clean(void) {
    for (uint8_t i=0; i<8; ++i) {
        if (MAX7219_decode_mode & (1 << i)) {
            MAX7219_write(i+1, 0x0F);  // blank in decode mode
        } else {
            MAX7219_write(i+1, 0x00);  // no segment is powered
        }
    }
}

void MAX7219_set_intensity(uint8_t intensity /* 0x00 - 0x0F */) {
    if (intensity >= 0 && intensity <= 0x0F) {
        MAX7219_write(0x0A, intensity);
    }
}

void MAX7219_set_default_config(void) {
    MAX7219_set_intensity(7);
    MAX7219_set_scan_limit(7);
    MAX7219_set_decode_mode(0x00);
    MAX7219_clean();
    MAX7219_normal_operation();
}

void MAX7219_print(char* string) {  // string should be \0 terminated (ASCIIZ string)

    if (MAX7219_decode_mode != 0x00) {
        MAX7219_set_no_decode_mode();
    }

    uint8_t buf[8];
    int index_buf = 0;
    int index_string = 0;

    while(index_buf < 8 && string[index_string] != '\0') {

        if (string[index_string] == '.') {

            if (index_buf == 0) {
                buf[index_buf] = 0x80;
            } else {
                buf[--index_buf] |= 0x80;
            }

        } else {

            for (uint8_t j=0; j<MAX7219_TABLE_SIZE; ++j) {
                if (MAX7219_table[j].character == string[index_string]) {
                    buf[index_buf] = MAX7219_table[j].code;
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
        MAX7219_write(index_buf-index_string, buf[index_string]);
    }

    // fill the rest of the screen with empty
    // index_buf still contains the buffer size
    // index_buf moved to the next element of the buffer if it is present int eh buffer   
    for (++index_buf; index_buf<=8; ++index_buf) {
        MAX7219_write(index_buf, 0);
    }
}

void MAX7219_print_long(int32_t sl) {
    //  -2147483648 .. 2147483647
    // extra chars will be cut
    char buf[12];
    ltoa(sl, buf, 10);
    MAX7219_print(buf);
}

void MAX7219_print_double(double d) {
    char buf[9];
    dtostrf(d, 8, MAX7219_DOUBLE_PREC, buf);
    MAX7219_print(buf);
}

