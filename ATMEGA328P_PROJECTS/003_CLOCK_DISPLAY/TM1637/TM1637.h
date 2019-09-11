#ifndef TM1637_H_
#define TM1637_H_

#include <stdint.h>
#include <stdbool.h>

#include "../pin_map/pin_type.h"

extern const uint8_t TM1637_CHAR_TABLE[];

typedef struct TM1637_driver_t
{
    PIN * clk;
    PIN * dio;
    const uint8_t buf_size; // is equal to 4 for clock display 
    uint8_t *buf;           // buffer of buf_size size 
    uint8_t brightness;     // 0 .. 7  
    bool screen_on;         // 0 (off) .. 1 (on), first buffer_write command will switch on 
                            //   the screen if 1 (on) is set 
} TM1637_driver;

void TM1637_send_buffer(TM1637_driver *driver);
void TM1637_init(TM1637_driver *driver);

#endif /* TM1637_H_ */
