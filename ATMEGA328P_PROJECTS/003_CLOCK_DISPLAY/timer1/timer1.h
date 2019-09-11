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

#define TIMER1_COUNTER_DIVIDER 125U
#define TIMER1_CLK_IO_DIVIDER TIMER1_CLK_IO_DIVIDER_1024
#define TIMER1_COUNTER_TOP_VALUE (OSCILLATOR_FREQUENCY / TIMER1_CLK_IO_DIVIDER / TIMER1_COUNTER_DIVIDER)

extern uint8_t timer1_task_trigger_counter;

void timer1_init();
void timer1_enable();
void timer1_disable();
void timer1_reset();

// TIMER1_TICK is in ms
#define TIMER1_TASK_TICK (1000 / TIMER1_COUNTER_TOP_VALUE) 
void timer1_task_8ms();

#endif /* TASKING_H_ */
