/*
  +--------------------------+
  | Stopwatch-timer solution |
  +--------------------------+

  This program is made to simplify time counting process on the kitchen.

  It consists of several modules:

  - Battery 18650 (~3.6 V)
  - Step up DC-DC converter (to 5 V)
  - On/Off switch (DPST)
  - Atmega328P (arduino nano board with removed stabilizer, CH340 chip and power leds) - main MCU
  - Button - simple button (SPST) with capacitor attached
  - 4 digits display with a colon in the middle (clock display) under TM1637 chip control
  - Buzzer, controlled by a transistor

*/

#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "main.h"

Buzzer buzzer0 = {.dio=&PD3};

Button button0 = {.dio=&PD2, 
                  .press_min_value=(40 / TIMER1_TASK_TICK), 
                  .long_press_top_value=(800 / TIMER1_TASK_TICK)};

TM1637_driver TM1637_driver0 = {.clk=&PC0, .dio=&PC1, .buf_size=4, .brightness=1, .screen_on=true};
Display display0 = {.driver=&TM1637_driver0};

// Interrupt service routines

ISR(TIMER1_COMPA_vect)
{
    timer1_task_trigger_counter ++;
}

ISR(INT0_vect, ISR_NOBLOCK)
{
    if (!PINR_CHECK(button0.dio))
    {
        switch (button0.state)
        {
            case BUTTON_KEY_UNDEFINED:
                button0.state = BUTTON_KEY_DOWN;
                break;

            default:
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
            case BUTTON_KEY_DOWN_COUNTING:
                // treat it like button was not pressed
                button0.state = BUTTON_KEY_UNDEFINED;
                break;

            case BUTTON_KEY_PRESS:  // button0 handler should switch 
                                    // key_down to key_pressed state in fault free case
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

    // Most simple sleep/wait for interrupt
    sleep_cpu();

    while (1)
    {
        if (timer1_task_trigger_counter)
        {
            timer1_task_trigger_counter = 0;
            timer1_task_8ms();
            if (timer1_task_trigger_counter)
            {
                // Very simple error handling
                
                // Disable timers (including buzzer timer, i.e. timer2)
                timer1_disable();
                buzzer0_disable();
                
                // Enable alert (red led)
                led_on(&led0);
                
                // Print current time counter to the display
                uint16_t _tcnt1 = TCNT1;  // save current counter
                display0.driver->buf[0] = TM1637_CHAR_TABLE[(uint8_t)(_tcnt1 >> 12)];
                display0.driver->buf[1] = TM1637_CHAR_TABLE[(uint8_t)(_tcnt1 >> 8)];
                display0.driver->buf[2] = TM1637_CHAR_TABLE[(uint8_t)(_tcnt1 >> 4)];
                display0.driver->buf[3] = TM1637_CHAR_TABLE[(uint8_t)(_tcnt1)];
                
                // Stop
                while (1)
                {
                    
                }
            }
        }

        sleep_cpu();
    }
}
