#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "PIN_MAP/avr_map.h"  // overrides <avr/io.h> pin definitions

#include "TM1637/TM1637.h"

#include "tasking.h"
#include "button0.h"
#include "display0.h"
#include "led0.h"
#include "buzzer0.h"


// Interrupt service routines

ISR(TIMER1_COMPA_vect)
{
    timer1_counter++;

    if (timer1_counter % (TIMER1_COUNTER_TOP_VALUE / 25) == 0)
    {
        task_switch.b.task_40ms_switch = 1;
    }

    if (timer1_counter % (TIMER1_COUNTER_TOP_VALUE / 5) == 0)
    {
        task_switch.b.task_200ms_switch = 1;
    }

    if (timer1_counter % (TIMER1_COUNTER_TOP_VALUE / 2) == 0)
    {
        task_switch.b.task_500ms_switch = 1;
    }

    if (timer1_counter == TIMER1_COUNTER_TOP_VALUE)
    {
        task_switch.b.task_1s_switch = 1;
        timer1_counter = 0;
    }
}

ISR(INT0_vect, ISR_NOBLOCK)
{
    if (!PIN_CHECK(PD2))
    {
        switch (button0_switch)
        {
            case KEY_UNDEFINED:
            {
                button0_switch = KEY_DOWN;
                break;
            }
            case KEY_UP:
            case KEY_DOWN:
            case KEY_PRESS:
            {
                break;
            }
        }
    }
    else
    {
        switch (button0_switch)
        {
            case KEY_UNDEFINED:
            case KEY_UP:
            {
                break;
            }
            case KEY_DOWN:  // handler for button0 was not executed
            {
                button0_switch = KEY_UNDEFINED;
                break;
            }
            case KEY_PRESS:
            {
                button0_switch = KEY_UP;
                break;
            }
        }
    }
}

int main(void)
{
    // Disable all interrupts
    cli();

    // Disable analog comparator
    ACSR |= (1 << ACD);

    // Initialize display driver
    TM1637_init(&PC0, &PC1);
    for (int i = 0; i < TM1637_BUF_SIZE; i++)
    {
        TM1637_buf[i] = TM1637_CHAR_TABLE[0];
    }
    TM1637_write_buffer();

    button0_configure();

    buzzer0_configure();

    led0_configure();

    // Configure timer for tasking routines
    timer1_configure();
    timer1_enable();

    sleep_enable();

    // Enable interrupts
    sei();

    // Most simple sleep
    sleep_cpu();

    while (1)
    {
        if (task_switch.b.task_40ms_switch)
        {
            task_40ms();
            task_switch.b.task_40ms_switch = 0;
        }

        if (task_switch.b.task_200ms_switch)
        {
            task_200ms();
            task_switch.b.task_200ms_switch = 0;
        }

        if (task_switch.b.task_500ms_switch)
        {
            task_500ms();
            task_500ms_counter++;
            task_switch.b.task_500ms_switch = 0;
        }

        if (task_switch.b.task_1s_switch)
        {
            task_1s();
            task_switch.b.task_1s_switch = 0;
        }

        sleep_cpu();
    }
}
