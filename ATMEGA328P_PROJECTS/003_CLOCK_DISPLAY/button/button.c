#include <avr/io.h>

#include "button.h"
#include "../main.h"

void button0_init()
{
    EICRA |= (1 << ISC00); // Any logical change on INT0 generates an interrupt request
    EIMSK |= (1 << INT0);  // Enable INT0

    DDR_CLEAR_P(button0.dio);  // as input
    PORTR_SET_P(button0.dio);  // pull up

    button0._press_counter = 0;
    button0._long_press_top_value = 100;  // 100 * 8 ms task = 800 ms
    button0._press_min_value = 5;         // 5   * 8 ms      = 40 ms
}

void button_handler(Button *button)
{
    switch (button->state)
    {
        case BUTTON_KEY_UNDEFINED:
            break;

        case BUTTON_KEY_DOWN:
            button->_press_counter = 0;
            button->state = BUTTON_KEY_DOWN_COUNTING;
            break;

        case BUTTON_KEY_DOWN_COUNTING:
            button->_press_counter ++;
            if (button->_press_min_value <= button->_press_counter)
            {
                button_on_key_down(button);
                button->state = BUTTON_KEY_PRESS;
            }
            break;

        case BUTTON_KEY_UP:
            button_on_key_up(button);
            if (button->_press_counter < button->_long_press_top_value)
            {
                button_on_press_short(button);
            }
            else
            {
                button_on_press_long(button);
            }
            button->state = BUTTON_KEY_UNDEFINED;
            break;

        case BUTTON_KEY_PRESS:
            if (button->_press_counter < button->_long_press_top_value)
            {
                button->_press_counter ++;
            }
            else
            {
                button->state = BUTTON_KEY_UP;
            }
            break;
    }
}

void button_on_key_down(Button *button)
{
    if (button == (Button *)(&button0))
    {
        led_on(&led0);
    }
}

void button_on_key_up(Button *button)
{
    if (button == (Button *)(&button0))
    {
        led_off(&led0);
    }
}

void button_on_press_short(Button *button)
{
    if (button == (Button *)(&button0))
    {
        display0_on_button0_click();
    }
}

void button_on_press_long(Button *button)
{
    if (button == (Button *)(&button0))
    {
        display0_reset();
    }
}
