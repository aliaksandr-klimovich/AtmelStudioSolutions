#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "pin_map/avr_map.h"
#include "main.h"

Buzzer buzzer0 = {.dio=&PD3};

Button button0 = {.dio=&PD2};

TM1637_driver TM1637_driver0 = {.clk=&PC0, .dio=&PC1, .buf_size=4, .brightness=1, .screen_on=true};
Display display0 = {.driver=&TM1637_driver0};

// Interrupt service routines

ISR(TIMER1_COMPA_vect)
{
    timer1_counter++;

    if (timer1_counter % (uint8_t)(TIMER1_COUNTER_TOP_VALUE * 0.04f) == 0)
        timer1_task_switch.b.t40ms = 1;

    if (timer1_counter % (uint8_t)(TIMER1_COUNTER_TOP_VALUE * 0.5f) == 0)
        timer1_task_switch.b.t500ms = 1;

    if (timer1_counter % (uint8_t)(TIMER1_COUNTER_TOP_VALUE * 1.0f) == 0)
        timer1_counter = 0;
}

ISR(INT0_vect, ISR_NOBLOCK)
{
    if (!PINR_CHECK_P(button0.dio))
    {
        switch (button0.state)
        {
            case BUTTON_KEY_UNDEFINED:
                button0.state = BUTTON_KEY_DOWN;
                break;

            case BUTTON_KEY_UP:
            case BUTTON_KEY_DOWN:
            case BUTTON_KEY_PRESS:
                break;
        }
    }
    else
    {
        switch (button0.state)
        {
            case BUTTON_KEY_UNDEFINED:
            case BUTTON_KEY_UP:
                break;

            case BUTTON_KEY_DOWN:  // handler for button0 was not executed
                // treat it like button was not pressed
                button0.state = BUTTON_KEY_UNDEFINED;
                break;

            case BUTTON_KEY_PRESS:  // button0 handler should switch key_down to key_pressed state in fault free case
                button0.state = BUTTON_KEY_UP;
                break;
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
    display_init(&display0);
    // And fill screen with zeros
    display_send_data(&display0);

    // Initialize button
    button0_init();

    // Initialize buzzer
    buzzer0_init();

    // Initialize led
    led_init(&led0);

    // Configure timer1 for tasking routines
    timer1_init();
    timer1_enable();

    sleep_enable();

    // Enable interrupts
    sei();

    // Most simple sleep / wait for interrupt
    sleep_cpu();

    while (1)
    {
        if (timer1_task_switch.b.t40ms)
        {
            timer1_task_40ms();
            timer1_task_switch.b.t40ms = 0;
        }

        if (timer1_task_switch.b.t500ms)
        {
            timer1_task_500ms();
            timer1_task_500ms_counter++;
            timer1_task_switch.b.t500ms = 0;
        }

        sleep_cpu();
    }
}
