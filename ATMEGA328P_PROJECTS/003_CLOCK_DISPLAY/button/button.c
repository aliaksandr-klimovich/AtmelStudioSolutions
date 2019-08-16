#include <avr/io.h>

#include "button.h"
#include "../main.h"


void button_init(Button *button)
{
    DDR_CLEAR_P(button->dio);  // Input
    PORTR_SET_P(button->dio);  // Enable pull up resistor

    EICRA |= (1 << ISC00); // Any logical change on INT0 generates an interrupt request
    EIMSK |= (1 << INT0);  // Enable INT0
    
    button->state = BUTTON_KEY_UNDEFINED;
    button->_press_counter = 0;
    button->_long_press_top_value = 15;  // 15 * ... = ... ms
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
            button_key_down(button);
            button->_press_counter = 0;
            button->state = BUTTON_KEY_PRESS;
            break;
        }

        case BUTTON_KEY_UP:
        {
            button_key_up(button);
            if ((1 <= button->_press_counter) && (button->_press_counter < button->_long_press_top_value))
            {
                button_press_short(button);
            }
            else
            {
                button_press_long(button);
            }
            button->state = BUTTON_KEY_UNDEFINED;
            break;
        }

        case BUTTON_KEY_PRESS:
        {
            button->_press_counter++;
            if (button->_press_counter == 255)
            {
                button->_press_counter = button->_long_press_top_value;
            }
            break;
        }
    }
}

void button_key_down(Button *button)
{
    if (button == (Button *)(&button0))
    {
        buzzer_enable(&buzzer0);   
    }
}

void button_key_up(Button *button)
{
    if (button == (Button *)(&button0))
    {
        buzzer_disable(&buzzer0); 
    }
}

void button_press_short(Button *button)
{
    if (button == (Button *)(&button0))
    {
        switch (display0.state)
        {
            case DISPLAY_COUNT_DIRECTION_UP:
            {
                display0.state = DISPLAY_COUNT_DIRECTION_DOWN;
                break;
            }
            case DISPLAY_COUNT_DIRECTION_DOWN:
            {
                display0.state = DISPLAY_COUNT_DIRECTION_UP;
                break;
            }
            case DISPLAY_COUNT_STOP:
            {
                timer1_reset();
                display0.state = DISPLAY_COUNT_START;
                break;
            }
            case DISPLAY_COUNT_START:
            case DISPLAY_COUNT_RESET:
            {
                break;
            }
        }
    }        
}

void button_press_long(Button *button)
{
    if (button == (Button *)(&button0))
    {
        display0.state = DISPLAY_COUNT_RESET;   
    }
}
