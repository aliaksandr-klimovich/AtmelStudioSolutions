#include "PCD8544.h"

void PCD8544_SPI_init(void) {
    PCD8544_DDR |= (1 << PCD8544_MOSI) | (1 << PCD8544_SCK) | (1 << PCD8544_RST) | (1 << PCD8544_SS) | (1 << PCD8544_DC);
    PCD8544_PORT |= (1 << PCD8544_RST) | (1 << PCD8544_SS);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void PCD8544_SPI_write(uint8_t data) {
    SPDR = data;
    while(!(SPSR & (1 << SPIF)));
}

void PCD8544_SS_enable(void) {
    PCD8544_PORT &= ~(1 << PCD8544_SS);
}

void PCD8544_SS_disable(void) {
    PCD8544_PORT |= (1 << PCD8544_SS);
}

void PCD8544_DC_data_mode(void) {
    PCD8544_PORT |= (1 << PCD8544_DC);
}

void PCD8544_DC_cmd_mode(void) {
    PCD8544_PORT &= ~(1 << PCD8544_DC);
}

void PCD8544_cmd(uint8_t cmd) {
    PCD8544_DC_cmd_mode();
    PCD8544_SS_enable();
    PCD8544_SPI_write(cmd);
    PCD8544_DC_data_mode();
    PCD8544_SS_disable();
}

void PCD8544_reset(void) {
    PCD8544_PORT &= ~(1 << PCD8544_RST);
    _delay_ms(100);
    PCD8544_PORT |= (1 << PCD8544_RST);
}

void PCD8544_init(void) {
    PCD8544_reset();
    PCD8544_cmd(0x21);      /* set command addition mode */
    PCD8544_cmd(0xC0);      /* set the voltage by sending C0, it means VOP = 5V */
    PCD8544_cmd(0x07);      /* set the temp. coefficient to 3 */
    PCD8544_cmd(0x13);      /* set value of Voltage Bias System */
    PCD8544_cmd(0x20);      /* set command basic mode */
    PCD8544_cmd(0x0C);      /* set display in normal mode */
}

void PCD8544_clear(void) {
    PCD8544_SS_enable();
    for (unsigned int i=0; i<=503; i++) {
        PCD8544_SPI_write(0);
    }
    PCD8544_SS_disable();
}

void PCD8544_set_XY(uint8_t x, uint8_t y) {
    PCD8544_cmd(x);
    PCD8544_cmd(y);
}

void PCD8544_text(char *text) {
    PCD8544_DC_data_mode();
    PCD8544_SS_enable();
    unsigned int text_len = strlen(text);
    for (unsigned int g=0; g<text_len; g++) {
        for (unsigned int i=0; i<5; i++) {
            PCD8544_SPI_write(ASCII[text[g] - 0x20][i]);
        }
        PCD8544_SPI_write(0x00);
    }
    PCD8544_SS_disable();
}

