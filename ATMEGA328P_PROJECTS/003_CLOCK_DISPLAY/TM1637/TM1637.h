#ifndef TM1637_H_
#define TM1637_H_

#include <stdint.h>

#include "../pin_map/pin_type.h"


#define TM1637_CMD_DATA_WRITE   0b01000000
#define TM1637_CMD_INIT_ADDR    0b11000000
#define TM1637_CMD_BRIGHTNESS   0b10000000
#define TM1637_CMD_SCREEN_ON    0b00001000
#define TM1637_CMD_SCREEN_OFF   0

#define TM1637_DELAY  0

#define TM1637_BUF_SIZE  4  // 4 stands here for clock display, max is 8 for this MCU
extern uint8_t TM1637_buf[TM1637_BUF_SIZE];

extern const uint8_t TM1637_CHAR_TABLE[];

extern uint8_t TM1637_brightness;
extern uint8_t TM1637_screen_on;


void TM1637_init(PIN * const clk, PIN * const dio);
void TM1637_send_buffer(void);
void TM1637_print(const char * s, ...);

#endif /* TM1637_H_ */
