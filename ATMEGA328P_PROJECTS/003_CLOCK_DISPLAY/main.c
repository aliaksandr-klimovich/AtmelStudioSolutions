#include "main.h"

Buzzer buzzer0 = {&PD3};

Button button0 = {&PD2};

Display display0 = {&PC0, &PC1};

// Interrupt service routines

ISR(TIMER1_COMPA_vect)
{
    timer1_counter++;

    if (timer1_counter % (uint8_t)(TIMER1_COUNTER_TOP_VALUE * 0.04f) == 0)
    {
        task_switch.b.task_40ms_switch = 1;
    }

    if (timer1_counter % (uint8_t)(TIMER1_COUNTER_TOP_VALUE * 0.2f) == 0)
    {
        task_switch.b.task_200ms_switch = 1;
    }

    if (timer1_counter % (uint8_t)(TIMER1_COUNTER_TOP_VALUE * 0.5f) == 0)
    {
        task_switch.b.task_500ms_switch = 1;
    }

    if (timer1_counter % (uint8_t)(TIMER1_COUNTER_TOP_VALUE * 1.f) == 0)
    {
        task_switch.b.task_1s_switch = 1;
        timer1_counter = 0;
    }
}

ISR(INT0_vect, ISR_NOBLOCK)
{
    if (!PINR_CHECK_P(button0.dio))
    {
        switch (button0.state)
        {
            case BUTTON_KEY_UNDEFINED:
            {
                button0.state = BUTTON_KEY_DOWN;
                break;
            }
            case BUTTON_KEY_UP:
            case BUTTON_KEY_DOWN:
            case BUTTON_KEY_PRESS:
            {
                break;
            }
        }
    }
    else
    {
        switch (button0.state)
        {
            case BUTTON_KEY_UNDEFINED:
            case BUTTON_KEY_UP:
            {
                break;
            }
            case BUTTON_KEY_DOWN:  // handler for button0 was not executed
            {
                button0.state = BUTTON_KEY_UNDEFINED;
                break;
            }
            case BUTTON_KEY_PRESS:
            {
                button0.state = BUTTON_KEY_UP;
                break;
            }
        }
    }
}

int main()
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
    TM1637_send_buffer();

    button_init(&button0);

    buzzer_init(&buzzer0);

    led_init(&led0);

    // Configure timer for tasking routines
    timer1_init();
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
