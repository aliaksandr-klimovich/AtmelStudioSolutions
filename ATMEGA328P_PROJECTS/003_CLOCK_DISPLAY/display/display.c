#include "display.h"
#include "../main.h"

uint8_t display0_min_top;
uint8_t display0_sec_top;

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

    if (display0.tc == (500 / TIMER1_TASK_TICK))
    {
        return DISPLAY_500_MS_TRIG;
    }
    else if (display0.tc == (1000 / TIMER1_TASK_TICK))
    {
        display0.tc = 0;
        return DISPLAY_1000_MS_TRIG;
    }

    return DISPLAY_COUNTING_TRIG;
}

void display0_handler()
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
            display0.tc = 0;
            display0.colon = true;
            if (display0_min_top == 0 && display0_sec_top == 0)
            {
                display0.min = 0;
                display0.sec = 0;
                display_send_data(&display0);
                display0.state = DISPLAY_COUNT_DIRECTION_UP;
            }
            else
            {
                display0.min = display0_min_top;
                display0.sec = display0_sec_top;
                display_send_data(&display0);
                display0.state = DISPLAY_COUNT_DIRECTION_DOWN;
            }                            
            break;

        case DISPLAY_COUNT_DIRECTION_UP:
            if (display0_min_top == 0 && display0_sec_top == 0)
            {
                // save top value
                display0_min_top = display0.min;
                display0_sec_top = display0.sec;
                // switch to count down
                display0.colon = true;
                display_send_data(&display0);
                display0.tc = 0;
                display0.state = DISPLAY_COUNT_DIRECTION_DOWN;
            }
            else 
            {
                // todo: error handler       
            }
            break;   

        case DISPLAY_COUNT_DIRECTION_DOWN:
            if (display0_min_top == 0 && display0_sec_top == 0)
            {
                // todo: error handler
            }
            else
            {
                // convert current and top value to seconds
                uint16_t total_sec_top = display0_sec_top + display0_min_top * 60;
                uint16_t total_sec = display0.sec + display0.min * 60;
                // get difference
                uint16_t total_dif = total_sec_top - total_sec;
                // convert it back to min and sec
                display0.min = total_dif / 60;
                display0.sec = total_dif % 60;
                // and save it also to the top value
                display0_min_top = display0.min;
                display0_sec_top = display0.sec;
                
                display0.colon = true;
                display_send_data(&display0);
                display0.tc = 0;
                //display0.state = DISPLAY_COUNT_DIRECTION_UP;
            }                
            break;
    }
}

void display0_reset()
{
    display0_min_top = 0;
    display0_sec_top = 0;
    display0.sec = 0;
    display0.min = 0;
    display0.colon = false;
    display0.state = DISPLAY_COUNT_STOP;
    display_send_data(&display0);
}

void display0_on_time_top()
{
    // will not be used
}

void display0_on_time_zero()
{
    buzzer0_3_short_clicks();
}
