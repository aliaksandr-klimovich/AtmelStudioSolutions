#include "display.h"
#include "../main.h"

void display_init(Display *display)
{
    TM1637_init(display->driver);

    display->state = DISPLAY_COUNT_STOP;
    display->min = 0;
    display->sec = 0;
    display->colon = false;
    display->tc = 0;
}

void display_send_data(Display *display)
{
    uint8_t minh = display->min / 10;
    uint8_t minl = display->min % 10;
    uint8_t sech = display->sec / 10;
    uint8_t secl = display->sec % 10;

    display->driver->buf[0] = TM1637_CHAR_TABLE[minh];
    display->driver->buf[1] = TM1637_CHAR_TABLE[minl];
    display->driver->buf[2] = TM1637_CHAR_TABLE[sech];
    display->driver->buf[3] = TM1637_CHAR_TABLE[secl];

    if (display->colon)
        display->driver->buf[1] |= 0x80;

    TM1637_send_buffer(display->driver);
}

DisplayTrigger display0_tc_handler()
{
    display0.tc ++;

    if (display0.tc == (500 / 8))
    {
        return DISPLAY_500_MS_TRIG;
    }
    else if (display0.tc == (1000 / 8))
    {
        display0.tc = 0;
        return DISPLAY_1000_MS_TRIG;
    }

    return DISPLAY_COUNTING_TRIG;
}

void display0_on_timer1_trigger()
{
    switch (display0.state)
    {
        case DISPLAY_COUNT_STOP:
            break;

        case DISPLAY_COUNT_DIRECTION_UP:
            switch (display0_tc_handler())
            {
                case DISPLAY_COUNTING_TRIG:
                    break;

                case DISPLAY_500_MS_TRIG:
                    display0.colon = false;
                    display_send_data(&display0);
                    break;

                case DISPLAY_1000_MS_TRIG:
                    if (++(display0.sec) > 59)
                    {
                        display0.sec = 0;
                        if (++(display0.min) > 99)
                        {
                            display0.sec = 59;
                            display0.min = 99;
                            display_send_data(&display0);
                            display0.state = DISPLAY_COUNT_STOP;
                            display0_on_time_top();
                            break;
                        }
                    }
                    display0.colon = true;
                    display_send_data(&display0);
                    break;
            }
            break;

        case DISPLAY_COUNT_DIRECTION_DOWN:
            switch (display0_tc_handler())
            {
                case DISPLAY_COUNTING_TRIG:
                    break;

                case DISPLAY_500_MS_TRIG:
                    display0.colon = false;
                    display_send_data(&display0);
                    break;

                case DISPLAY_1000_MS_TRIG:
                    if (--display0.sec < 0)
                    {
                        display0.sec = 59;
                        display0.min --;
                    }

                    if ((display0.min == 0 && display0.sec == 0) || display0.min < 0)
                    {
                        display0.sec = 0;
                        display0.min = 0;
                        display0.colon = false;
                        display_send_data(&display0);
                        display0.state = DISPLAY_COUNT_STOP;
                        display0_on_time_zero();
                        break;
                    }

                    display0.colon = true;
                    display_send_data(&display0);
                    break;
            }
            break;
    }
}

void display0_on_button0_click()
{
    switch (display0.state)
    {
        case DISPLAY_COUNT_STOP:
            display0.min = 0;
            display0.sec = 0;
            display0.colon = true;
            display_send_data(&display0);
            display0.tc = 0;
            display0.state = DISPLAY_COUNT_DIRECTION_UP;
            break;

        case DISPLAY_COUNT_DIRECTION_UP:
            display0.colon = true;
            display_send_data(&display0);
            display0.tc = 0;
            display0.state = DISPLAY_COUNT_DIRECTION_DOWN;
            break;

        case DISPLAY_COUNT_DIRECTION_DOWN:
            display0.colon = true;
            display_send_data(&display0);
            display0.tc = 0;
            display0.state = DISPLAY_COUNT_DIRECTION_UP;
            break;
    }
}

void display0_reset()
{
    display0.sec = 0;
    display0.min = 0;
    display0.colon = false;
    display0.state = DISPLAY_COUNT_STOP;
    display_send_data(&display0);
}

void display0_on_time_top()
{

}

void display0_on_time_zero()
{

}
