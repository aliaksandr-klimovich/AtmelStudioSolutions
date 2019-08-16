#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "pin_map/avr_map.h"
#include "TM1637/TM1637.h"
#include "main.h"


Buzzer buzzer0 = {&PD3};

Button button0 = {&PD2};

Display display0 = {&PC0, &PC1};

// Interrupt service routines

ISR(TIMER1_COMPA_vect)
{
    timer1_counter++;

    if (timer1_counter % (TIMER1_COUNTER_TOP_VALUE / 25 /*40ms*/) == 0)
    {
        timer1_task_switch.b.t40ms = 1;
    }
    
    if (timer1_counter % (TIMER1_COUNTER_TOP_VALUE / 2 /*500ms*/) == 0)
    {
        timer1_task_switch.b.t500ms = 1;
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
    display_init(&display0);
    // And fill screen with zeros
    display_send_data(&display0);

    // Initialize button
    button_init(&button0);

    // Initialize buzzer
    buzzer_init(&buzzer0);

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
