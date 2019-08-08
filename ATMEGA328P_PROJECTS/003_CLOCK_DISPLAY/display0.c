#include "display0.h"

DisplayState display0_state = DISPLAY_COUNT_STOP;
int8_t display0_min = 0;
int8_t display0_sec = 0;

void display0_set(uint8_t colon)
{
    if (colon)
    {
        TM1637_print("%02u:%02u", display0_min, display0_sec);
    }
    else
    {
        TM1637_print("%02u %02u", display0_min, display0_sec);
    }
}

inline void display0_handler()
{
    switch (display0_state)
    {
        case DISPLAY_COUNT_STOP:
        {
            break;
        }
        case DISPLAY_COUNT_START:
        {
            display0_start();
            display0_state = DISPLAY_COUNT_DIRECTION_UP;
            // no break;
        }
        case DISPLAY_COUNT_DIRECTION_UP:
        {
            if (task_500ms_counter % 2)
            {
                display0_set(0);
                if (++display0_sec >= 60)
                {
                    display0_sec = 0;
                    if (++display0_min >= 60)
                    {
                        display0_min = 0;
                    }
                }
            }
            else
            {
                display0_set(1);
            }
            break;
        }
        case DISPLAY_COUNT_DIRECTION_DOWN:
        {
            if (task_500ms_counter % 2)
            {
                display0_set(0);
                if (--display0_sec < 0)
                {
                    display0_sec = 59;
                    if (--display0_min < 0)
                    {
                        display0_min = 0;
                        display0_sec = 0;
                        display0_state = DISPLAY_COUNT_STOP;
                        display0_time_out();
                    }
                }
            }
            else
            {
                display0_set(1);
            }
            break;
        }
        case DISPLAY_COUNT_RESET:
        {
            if (task_500ms_counter % 2)
            {
                display0_min = 0;
                display0_sec = 0;
                display0_state = DISPLAY_COUNT_START;
                display0_reset();
            }
            break;
        }
    }
}

inline void display0_reset()
{

}

inline void display0_time_out()
{
    led0_on();
}

inline void display0_start()
{
    led0_off();
}
