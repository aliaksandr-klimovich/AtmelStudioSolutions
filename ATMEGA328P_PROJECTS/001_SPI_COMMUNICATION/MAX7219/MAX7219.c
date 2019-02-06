#include "MAX7219.h"

uint8_t _MAX7219_decode_mode = 0;

typedef struct {
    char c;
    uint8_t code;
} _MAX7219_table;

static const _MAX7219_table MAX7219_table[] = {
    {'0', 0b01111110},  // O, D?
    {'1', 0b00110000},
    {'2', 0b01101101},
    {'3', 0b01111001},
    {'4', 0b00110011},
    {'5', 0b01011011},  // S
    {'6', 0b01011111},
    {'7', 0b01110000},
    {'8', 0b01111111},  // B?
    {'9', 0b01111011},
    {'-', 0b00000001},
    {' ', 0b00000000},
    {'H', 0b00110111},  // X? K?
    {'E', 0b01001111},
    {'L', 0b00001110},
    {'P', 0b01100111},
    {'_', 0b00001000},
    {'U', 0b00111110},
    {'F', 0b01000111},
    {'C', 0b01001110},
    {'d', 0b00111101},
    {'b', 0b00011111},
    {'A', 0b01110111},
};

void MAX7219_init(void) {
    MAX7219_DDR |= (1 << MAX7219_DIN) | (1 << MAX7219_CS) | (1 << MAX7219_CLK);
    MAX7219_PORT |= (1 << MAX7219_CS);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void MAX7219_write(uint8_t addr, uint8_t data) {
    MAX7219_PORT &= ~(1 << MAX7219_CS);
    SPDR = addr;
    while(!(SPSR & (1 << SPIF)));
    SPDR = data;
    while(!(SPSR & (1 << SPIF)));
    MAX7219_PORT |= (1 << MAX7219_CS);
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
    MAX7219_write(0x0B, sl);
}

void MAX7219_set_decode_mode(uint8_t dm /* 0x00 - 0xFF */) {
    MAX7219_write(0x09, dm);
    _MAX7219_decode_mode = dm;
    // do not forget to clean the screen
}

void MAX7219_set_no_decode_mode(void) {
    MAX7219_write(0x09, 0x00);
    _MAX7219_decode_mode = 0;
    // do not forget to clean the screen
}

void MAX7219_clean(void) {
    for (uint8_t i=0; i<8; ++i) {
        if (_MAX7219_decode_mode & (1 << i)) {
            MAX7219_write(i+1, 0x0F);  // blank in decode mode
        } else {
            MAX7219_write(i+1, 0x00);  // no segment is powered
        }
    }
}

void MAX7219_set_intensity(uint8_t intensity /* 0x00 - 0x0F */) {
    MAX7219_write(0x0A, intensity);
}

void MAX7219_set_default_config(void) {
    MAX7219_set_intensity(7);
    MAX7219_set_scan_limit(7);
    MAX7219_set_decode_mode(0xFF);
    MAX7219_clean();
    MAX7219_normal_operation();
}

void MAX7219_print(char* s) {  // s should \0 terminated (ASCIIZ string)
    
    if (_MAX7219_decode_mode != 0x00) {
        MAX7219_set_decode_mode(0x00);
    }
    
    uint8_t buf[8];
    int i_buf = 0;
    int i_s = 0;
    
    while(i_buf < 8 && s[i_s] != '\0') {

        if (s[i_s] == '.') {
            
            if (i_buf == 0) {
                buf[i_buf] = 0x80;
            } else {                
                buf[--i_buf] |= 0x80;
            }
        
        } else {

            for (uint8_t j=0; j<(sizeof(MAX7219_table) / sizeof(MAX7219_table[0])); ++j) {
                if (MAX7219_table[j].c == s[i_s]) {
                    buf[i_buf] = MAX7219_table[j].code;
                    break;
                }
            }
        }        
        
        ++i_buf;
        ++i_s;
    }
    
    if (s[i_s] == '.') {
        buf[7] |= 0x80;
    }
    
    for (i_s=0; i_s<i_buf; ++i_s) {
        MAX7219_write(i_buf-i_s, buf[i_s]);  
    } 
    
    for (++i_buf; i_buf<=8; ++i_buf) {
        MAX7219_write(i_buf, 0); 
    }  
       
}

