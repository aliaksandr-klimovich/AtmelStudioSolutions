#include <avr/io.h>

#include "button.h"
#include "../main.h"


void button0_init()
{
    DDR_CLEAR_P(button0.dio);  // Input
    PORTR_SET_P(button0.dio);  // Enable pull up resistor

    // todo: remove hard code: move overall config to separate class function
    EICRA |= (1 << ISC00); // Any logical change on INT0 generates an interrupt request
    EIMSK |= (1 << INT0);  // Enable INT0

    button0.state = BUTTON_KEY_UNDEFINED;
    button0._press_counter = 0;
    button0._long_press_top_value = 15;  // 15 * 40 ms task = 600 ms
}

void button_handler(Button *button)
{
    switch (button->state)
    {
        case BUTTON_KEY_UNDEFINED:
        {
            break;
        }

        case BUTTON_KEY_DOWN:
        {
            button_on_key_down(button);
            button->_press_counter = 0;
            button->state = BUTTON_KEY_PRESS;
            break;
        }

        case BUTTON_KEY_UP:
        {
            button_on_key_up(button);
            if ((1 <= button->_press_counter) && (button->_press_counter < button->_long_press_top_value))
            {
                button_on_press_short(button);
            }
            else
            {
                button_on_press_long(button);
            }
            button->state = BUTTON_KEY_UNDEFINED;
            break;
        }

        case BUTTON_KEY_PRESS:
        {
            if (button->_press_counter < button->_long_press_top_value)
            {
                button->_press_counter++;
            }
            break;
        }
    }
}

void button_on_key_down(Button *button)
{
    if (button == (Button *)(&button0))
    {

    }
}

void button_on_key_up(Button *button)
{
    if (button == (Button *)(&button0))
    {
        
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
