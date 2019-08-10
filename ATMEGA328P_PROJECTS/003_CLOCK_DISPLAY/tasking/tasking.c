#include  "tasking.h"

const uint8_t TIMER1_COUNTER_TOP_VALUE = OSCILLATOR_FREQUENCY / TIMER1_CLK_IO_DIVIDER / 625UL;  // 625 should be set in `timer1_configure` function
uint8_t timer1_counter;
TaskSwitch task_switch;
uint8_t task_500ms_counter;

void timer1_configure()
{
    TCCR1B |= (1<< WGM12);  // Set CTC mode
    OCR1AH = 0x02;
    OCR1AL = 0x71 - 1;  // 625 - 1
    TIMSK1 |= (1 << OCIE1A);
}

void timer1_enable()
{
    TCCR1B |= (1 << CS10) | (1 << CS12);  // 1024
}

void task_40ms()
{
    button_handler(&button0);
}

void task_200ms()
{

}

void task_500ms()
{
    display_handler(&display0);
}

void task_1s()
{

}
