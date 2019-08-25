#include  "timer1.h"
#include "../main.h"

uint8_t timer1_counter;
TaskSwitch timer1_task_switch;
uint8_t timer1_task_500ms_counter;

void timer1_init()
{
    TCCR1B = (1 << WGM12);  // Set CTC mode
    OCR1AH = TIMER1_COUNTER_DIVIDER / 0x100;
    OCR1AL = TIMER1_COUNTER_DIVIDER % 0x100 - 1;
    TIMSK1 = (1 << OCIE1A);
}

void timer1_enable()
{
#if TIMER1_CLK_IO_DIVIDER == TIMER1_CLK_IO_DIVIDER_1024
    TCCR1B = TIMER1_PRESCALER_1024 | (1 << WGM12);
#elif TIMER1_CLK_IO_DIVIDER == TIMER1_CLK_IO_DIVIDER_256
    TCCR1B = TIMER1_PRESCALER_256 | (1 << WGM12);
#endif
} 

void timer1_disable()
{
    TCCR1B = TIMER1_PRESCALER_0 | (1 << WGM12);
}

void timer1_reset()
{
    TCNT1 = 0;
    timer1_counter = 0;
}

void timer1_task_40ms()
{
    button_handler(&button0);
}

void timer1_task_500ms()
{
    display0_on_timer1_trigger(&display0);
}
