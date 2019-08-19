#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

#include "../pin_map/pin_type.h"


typedef enum DisplayState_t
{
    DISPLAY_COUNT_STOP,
    DISPLAY_COUNT_DIRECTION_UP,
    DISPLAY_COUNT_DIRECTION_DOWN,
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

void display0_on_timer1_trigger();
void display0_on_button0_click();
void display0_start();
void display0_reset();
void display0_on_time_top();
void display0_on_time_zero();

#endif /* DISPLAY_H_ */