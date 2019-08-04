/*
* button0.c
*
* Created: 04.08.2019 17:17:13
*  Author: Aliaksandr
*/

#include "button0.h"

const uint8_t BUTTON0_LONG_PRESS_COUNTER_TOP_VALUE = 15;  // 15 * 40ms = 600ms
ButtonSwitch button0_switch = KEY_UNDEFINED;
uint8_t button0_press_counter = 0;

inline void button0_handler()
{
    switch (button0_switch)
    {
        case KEY_UNDEFINED:
        {
            break;
        }
        
        case KEY_DOWN:
        {
            button0_key_down();
            button0_press_counter = 0;
            button0_switch = KEY_PRESS;
            break;
        }

        case KEY_UP:
        {
            button0_key_up();
            if (1 <= button0_press_counter && button0_press_counter < BUTTON0_LONG_PRESS_COUNTER_TOP_VALUE)
            {
                button0_press_short();
            }
            else
            {
                button0_press_long();
            }
            button0_switch = KEY_UNDEFINED;
            break;
        }

        case KEY_PRESS:
        {
            button0_press_counter++;
            if (button0_press_counter == 255)
            {
                button0_press_counter = BUTTON0_LONG_PRESS_COUNTER_TOP_VALUE;
            }
            break;
        }
    }
}

inline void button0_key_down()
{

}

inline void button0_key_up()
{

}

inline void button0_press_short()
{
    switch (display0_state)
    {
        case DISPLAY_COUNT_DIRECTION_UP:
        {
            display0_state = DISPLAY_COUNT_DIRECTION_DOWN;
            break;
        }
        case DISPLAY_COUNT_DIRECTION_DOWN:
        {
            display0_state = DISPLAY_COUNT_DIRECTION_UP;
            break;
        }
        case DISPLAY_COUNT_STOP:
        {
            display0_state = DISPLAY_COUNT_START;
            break;
        }
        case DISPLAY_COUNT_START:
        case DISPLAY_COUNT_RESET:
        {
            break;
        }
    }
}

inline void button0_press_long()
{
    display0_state = DISPLAY_COUNT_RESET;
}

inline void button0_configure()
{
    DDR_CLEAR(PD2);  // PD2 as input
    PORT_SET(PD2);  // Enable pull up resistor
    
    EICRA |= (1 << ISC00); // Any logical change on INT0 generates an interrupt request
    EIMSK |= (1 << INT0);  // Enable INT0
}
