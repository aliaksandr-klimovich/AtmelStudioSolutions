/*
 * tasking.c
 *
 * Created: 04.08.2019 18:05:11
 *  Author: Aliaksandr
 */ 

#include  "tasking.h"

const uint8_t TIMER1_COUNTER_TOP_VALUE = OSCILLATOR_FREQUENCY / TIMER1_CLK_IO_DIVIDER / 625UL;  // 625 should be set in `timer1_configure` function

inline void timer1_configure()
{
    TCCR1B |= (1<< WGM12);  // Set CTC mode
    OCR1AH = 0x02;
    OCR1AL = 0x71;  // 625
    TIMSK1 |= (1 << OCIE1A);
}

inline void timer1_enable()
{
    TCCR1B |= (1 << CS10) | (1 << CS12);  // 1024
}

inline void task_40ms()
{
    button0_handler();
}

inline void task_200ms()
{

}

inline void task_500ms()
{
    display0_handler();
}

inline void task_1s()
{

}
