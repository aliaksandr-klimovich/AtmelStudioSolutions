#include  "tasking.h"

uint8_t timer1_counter;
TaskSwitch task_switch;

uint8_t task_500ms_counter;

void timer1_init()
{
    TCCR1B |= (1 << WGM12);  // Set CTC mode
    OCR1AH = TIMER1_COUNTER_DIVIDER / 0x100;
    OCR1AL = TIMER1_COUNTER_DIVIDER % 0x100 - 1;
    TIMSK1 |= (1 << OCIE1A);
}

void timer1_enable()
{
#if TIMER1_CLK_IO_DIVIDER == TIMER1_CLK_IO_DIVIDER_1024
    TCCR1B |= TIMER1_PRESCALER_1024;
#elif TIMER1_CLK_IO_DIVIDER == TIMER1_CLK_IO_DIVIDER_256
    TCCR1B |= TIMER1_PRESCALER_256;
#endif
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
