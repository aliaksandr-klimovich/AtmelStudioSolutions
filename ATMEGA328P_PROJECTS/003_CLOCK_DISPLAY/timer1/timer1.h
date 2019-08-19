#ifndef TASKING_H_
#define TASKING_H_

#include <stdint.h>

#include <avr/io.h>


enum TIMER1_PRESCALER_t
{
    TIMER1_PRESCALER_0    = (0 << CS12) | (0 << CS11) | (0 << CS10),  // timer1 disconnected
    TIMER1_PRESCALER_1    = (0 << CS12) | (0 << CS11) | (1 << CS10),  // clk / 1 (no prescaling)
    TIMER1_PRESCALER_8    = (0 << CS12) | (1 << CS11) | (0 << CS10),  // clk / 8
    TIMER1_PRESCALER_64   = (0 << CS12) | (1 << CS11) | (1 << CS10),  // clk / 64
    TIMER1_PRESCALER_256  = (1 << CS12) | (0 << CS11) | (0 << CS10),  // clk / 256
    TIMER1_PRESCALER_1024 = (1 << CS12) | (0 << CS11) | (1 << CS10),  // clk / 1024
};

enum TIMER1_CLK_IO_DIVIDER_t
{
    TIMER1_CLK_IO_DIVIDER_0    = 0,
    TIMER1_CLK_IO_DIVIDER_1    = 1,
    TIMER1_CLK_IO_DIVIDER_8    = 8,
    TIMER1_CLK_IO_DIVIDER_64   = 64,
    TIMER1_CLK_IO_DIVIDER_256  = 256,
    TIMER1_CLK_IO_DIVIDER_1024 = 1024,
};

typedef union TaskSwitch_t
{
    struct
    {
        uint8_t t2s:    1;
        uint8_t t1s:    1;
        uint8_t t500ms: 1;
        uint8_t bit3:   1;
        uint8_t t40ms:  1;
        uint8_t bit5:   1;
        uint8_t bit6:   1;
        uint8_t bit7:   1;
    } b;
    uint8_t u8;
} TaskSwitch;

#define TIMER1_COUNTER_DIVIDER 125U
#define TIMER1_CLK_IO_DIVIDER TIMER1_CLK_IO_DIVIDER_1024
#define TIMER1_COUNTER_TOP_VALUE (OSCILLATOR_FREQUENCY / TIMER1_CLK_IO_DIVIDER / TIMER1_COUNTER_DIVIDER)

extern uint8_t timer1_counter;
extern TaskSwitch timer1_task_switch;
extern uint8_t timer1_task_500ms_counter;


void timer1_init();
void timer1_enable();
void timer1_disable();
void timer1_reset();

void timer1_task_40ms();
void timer1_task_500ms();

#endif /* TASKING_H_ */
