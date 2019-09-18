#include <avr/io.h>

#include "button.h"
#include "../main.h"

void button_init(Button *button)
{
    PIN_AS_INPUT_PULLUP(button->dio); 
    button->press_counter = 0;
    // .press_min_value and .long_press_top_value should be set outside this function
}    

void button0_init()
{
    EICRA |= (1 << ISC00); // Any logical change on INT0 generates an interrupt request 
    EIMSK |= (1 << INT0);  // Enable INT0 

    button_init(&button0);
}

void button_handler(Button *button)
{
    switch (button->state)
    {
        case BUTTON_KEY_UNDEFINED:
            break;

        case BUTTON_KEY_DOWN:
            button->press_counter = 0;
            button->state = BUTTON_KEY_DOWN_COUNTING;
            break;

        case BUTTON_KEY_DOWN_COUNTING:
            button->press_counter ++;
            if (button->press_min_value <= button->press_counter)
            {
                button_on_key_down(button);
                button->state = BUTTON_KEY_PRESS;
            }
            break;

        case BUTTON_KEY_UP:
            button_on_key_up(button);
            if (button->press_counter < button->long_press_top_value)
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
            if (button->press_counter < button->long_press_top_value)
            {
                button->press_counter ++;
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

}

void button_on_key_up(Button *button)
{

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
        buzzer0_1_long_click();
    }
}
