/*
 * display.h
 *
 * Created: 04.08.2019 17:40:40
 *  Author: Aliaksandr
 */ 


#ifndef DISPLAY0_H_
#define DISPLAY0_H_

#include <stdint.h>
#include "TM1637/TM1637.h"
#include "tasking.h"
#include "led0.h"

typedef enum DisplayState_t
{
    DISPLAY_COUNT_STOP,
    DISPLAY_COUNT_START,
    DISPLAY_COUNT_DIRECTION_UP,
    DISPLAY_COUNT_DIRECTION_DOWN,
    DISPLAY_COUNT_RESET,
} DisplayState;

DisplayState display0_state;

void display0_set(uint8_t colon);
void display0_time_out();
void display0_reset();
void display0_handler();
void display0_start();

#endif /* DISPLAY0_H_ */