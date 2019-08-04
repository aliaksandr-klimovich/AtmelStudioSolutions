/*
 * tasking.h
 *
 * Created: 04.08.2019 18:04:25
 *  Author: Aliaksandr
 */ 


#ifndef TASKING_H_
#define TASKING_H_

#include <stdint.h>
#include "PIN_MAP/avr_map.h"
#include "config.h"
#include "display0.h"
#include "button0.h"

#define TIMER1_CLK_IO_DIVIDER 1024UL
const uint8_t TIMER1_COUNTER_TOP_VALUE;

uint8_t timer1_counter;

void timer1_configure();
void timer1_enable();

typedef union TaskSwitch_t
{
    struct
    {
        uint8_t task_1s_switch: 1;
        uint8_t task_500ms_switch: 1;
        uint8_t task_200ms_switch: 1;
        uint8_t task_40ms_switch: 1;
        uint8_t bit4: 1;
        uint8_t bit5: 1;
        uint8_t bit6: 1;
        uint8_t bit7: 1;
    } b;
    uint8_t u8;
} TaskSwitch;

TaskSwitch task_switch;
uint8_t task_500ms_counter;

void task_40ms();
void task_200ms();
void task_500ms();
void task_1s();

#endif /* TASKING_H_ */