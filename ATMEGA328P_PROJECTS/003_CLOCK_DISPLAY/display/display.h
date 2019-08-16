#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

#include "../pin_map/pin_type.h"


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
    int8_t min;
    int8_t sec;
    uint8_t colon;
} Display;

void display_init(Display *display);
void display_send_data(Display *display);
void display_handler(Display *display);
void display_start(Display *display);
void display_reset(Display *display);
void display_time_out(Display *display);

#endif /* DISPLAY_H_ */