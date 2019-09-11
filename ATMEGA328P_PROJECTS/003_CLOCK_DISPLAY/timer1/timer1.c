#include  "timer1.h"
#include "../main.h"

uint8_t timer1_task_trigger_counter;

void timer1_init()
{
    TCCR1B = (1 << WGM12);  // Set CTC mode 
    OCR1A = TIMER1_COUNTER_DIVIDER;
    TIMSK1 = (1 << OCIE1A);
}

inline void timer1_enable()
{
#if TIMER1_CLK_IO_DIVIDER == TIMER1_CLK_IO_DIVIDER_1024
    TCCR1B = TIMER1_PRESCALER_1024 | (1 << WGM12);
#elif TIMER1_CLK_IO_DIVIDER == TIMER1_CLK_IO_DIVIDER_256
    TCCR1B = TIMER1_PRESCALER_256 | (1 << WGM12);
#endif
}

inline void timer1_disable()
{
    TCCR1B = TIMER1_PRESCALER_0 | (1 << WGM12);
}

inline void timer1_reset()
{
    TCNT1 = 0;
}

/*#define timer1_skipping \
    if (TCNT1 > ((OCR1A * 80) / 100)) \
        return;*/

void timer1_task_8ms()
{
    // TODO: task management
    display0_handler();
    button_handler(&button0);
    buzzer0_handler();
}
