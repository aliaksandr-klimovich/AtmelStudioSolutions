#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include "../pin_map/pin_map.h"
#include "../TM1637/TM1637.h"

typedef enum DisplayState_t
{
    DISPLAY_COUNT_STOP,
    DISPLAY_COUNT_START,
    DISPLAY_COUNT_DIRECTION_UP,
    DISPLAY_COUNT_DIRECTION_DOWN,
    DISPLAY_COUNT_RESET,
} DisplayState;

typedef struct Display_t
{
    PIN *clk;
    PIN *dio;
    DisplayState state;
    uint8_t min;
    uint8_t sec;
    uint8_t colon;
} Display;

void display_init(Display *display);
void display_send_data(Display *display);
void display_handler(Display *display);
void display_time_out();
void display_reset();
void display_start();

// External dependencies
#include "../main.h"

#endif /* DISPLAY_H_ */