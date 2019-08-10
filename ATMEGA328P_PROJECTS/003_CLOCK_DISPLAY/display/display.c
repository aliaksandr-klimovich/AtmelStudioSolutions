#include "display.h"

void display_init(Display *display)
{
    display->state = DISPLAY_COUNT_STOP;
    display->min = 0;
    display->sec = 0;
    display->colon = 0;
}

void display_send_data(Display *display)
{
    uint8_t minh = display->min / 10;
    uint8_t minl = display->min % 10;
    uint8_t sech = display->sec / 10;
    uint8_t secl = display->sec % 10;

    TM1637_buf[0] = TM1637_CHAR_TABLE[minh];
    TM1637_buf[1] = TM1637_CHAR_TABLE[minl];
    TM1637_buf[2] = TM1637_CHAR_TABLE[sech];
    TM1637_buf[3] = TM1637_CHAR_TABLE[secl];

    if (display->colon)
    {
        TM1637_buf[1] |= 0x80;
    }

    TM1637_send_buffer();
}

void display_handler(Display *display)
{
    switch (display->state)
    {
        case DISPLAY_COUNT_STOP:
        {
            break;
        }

        case DISPLAY_COUNT_START:
        {
            display->state = DISPLAY_COUNT_DIRECTION_UP;
            display_start(display);
            // no break;
        }

        case DISPLAY_COUNT_DIRECTION_UP:
        {
            if (task_500ms_counter % 2)
            {
                display->colon = 0;
                display_send_data(display);
                if (++display->sec >= 60)
                {
                    display->sec = 0;
                    if (++display->min >= 60)
                    {
                        display->min = 0;
                    }
                }
            }
            else
            {
                display->colon = 1;
                display_send_data(display);
            }
            break;
        }

        case DISPLAY_COUNT_DIRECTION_DOWN:
        {
            if (task_500ms_counter % 2)
            {
                display->colon = 0;
                display_send_data(display);
                if (--display->sec < 0)
                {
                    display->sec = 59;
                    if (--display->min < 0)
                    {
                        display->min = 0;
                        display->sec = 0;
                        display->state = DISPLAY_COUNT_STOP;
                        display_time_out(display);
                    }
                }
            }
            else
            {
                display->colon = 1;
                display_send_data(display);
            }
            break;
        }

        case DISPLAY_COUNT_RESET:
        {
            if (task_500ms_counter % 2)
            {
                display->min = 0;
                display->sec = 0;
                display->state = DISPLAY_COUNT_START;
                display_reset(display);
            }
            break;
        }
    }
}

void display_start(Display *display)
{
    if (display == (Display *)(&display0))
    {
        led_off(&led0);
    }
}

void display_reset(Display *display)
{
    if (display == (Display *)(&display0))
    {

    }
}

void display_time_out(Display *display)
{
    if (display == (Display *)(&display0))
    {
        led_on(&led0);
    }
}
