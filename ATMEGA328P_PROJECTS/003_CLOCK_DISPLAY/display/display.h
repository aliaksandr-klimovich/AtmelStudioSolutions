#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>

#include "../TM1637/TM1637.h"

typedef enum DisplayState_t
{
    DISPLAY_COUNT_STOP,
    DISPLAY_COUNT_DIRECTION_UP,
    DISPLAY_COUNT_DIRECTION_DOWN,
} DisplayState;

typedef enum DisplayTrigger_t
{
    DISPLAY_COUNTING_TRIG,
    DISPLAY_500_MS_TRIG,
    DISPLAY_1000_MS_TRIG,
} DisplayTrigger;

typedef struct Display_t
{
    TM1637_driver *driver;
    DisplayState state;
    int8_t min;
    int8_t sec;
    bool colon;
    int16_t tc;
} Display;

void display_init(Display *display);
void display_send_data(Display *display);

void display0_reset();
void display0_on_timer1_trigger();
void display0_on_button0_click();
void display0_on_time_top();
void display0_on_time_zero();

#endif /* DISPLAY_H_ */
