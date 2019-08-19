#include "display.h"
#include "../TM1637/TM1637.h"
#include "../main.h"


void display_init(Display *display)
{
    TM1637_init(display->clk, display->dio);
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

void display0_on_timer1_trigger()
{
    switch (display0.state)
    {
        case DISPLAY_COUNT_STOP:
        {
            break;
        }

        case DISPLAY_COUNT_DIRECTION_UP:
        {
            if (timer1_task_500ms_counter % 2)
            {
                display0.colon = 0;
            }
            else  /* first here */
            {
                if (++(display0.sec) > 59)
                {
                    display0.sec = 0;
                    if (++(display0.min) > 99)
                    {
                        display0.sec = 59;
                        display0.min = 99;
                        display0.colon = 0;
                        display_send_data(&display0);
                        display0.state = DISPLAY_COUNT_STOP;
                        display0_on_time_top();
                        break;
                    }
                }
                display0.colon = 1;
            }
            display_send_data(&display0);
            break;
        }

        case DISPLAY_COUNT_DIRECTION_DOWN:
        {
            if (timer1_task_500ms_counter % 2)
            {
                display0.colon = 0;
            }
            else  /* first here */
            {
                if (--display0.sec < 0)
                {
                    display0.sec = 59;
                    display0.min --;
                }
                
                if (display0.min <= 0 && display0.sec <= 0)
                {
                    display0.sec = 0;
                    display0.min = 0;
                    display0.colon = 0;
                    display_send_data(&display0);
                    display0.state = DISPLAY_COUNT_STOP;
                    display0_on_time_zero();
                    break;
                }
                
                display0.colon = 1;
            }
            display_send_data(&display0);
            break;
        }
    }
}

void display0_on_button0_click()
{
    switch (display0.state)
    {
        case DISPLAY_COUNT_STOP:
        {
            timer1_disable();
            timer1_reset();
            display0.min = 0;
            display0.sec = -1;
            timer1_task_500ms_counter = 0;
            display0.state = DISPLAY_COUNT_DIRECTION_UP;
            display0_on_timer1_trigger();
            timer1_task_500ms_counter ++;
            timer1_enable();
            led_off(&led0);
            break;
        }

        case DISPLAY_COUNT_DIRECTION_UP:
        {
            timer1_disable();
            timer1_reset();
            timer1_task_500ms_counter = 0;
            display0.sec++;
            display0.state = DISPLAY_COUNT_DIRECTION_DOWN;
            display0_on_timer1_trigger();
            timer1_task_500ms_counter ++;
            timer1_enable();
            break;
        }

        case DISPLAY_COUNT_DIRECTION_DOWN:
        {
            timer1_disable();
            timer1_reset();
            timer1_task_500ms_counter = 0;
            display0.sec--;
            display0.state = DISPLAY_COUNT_DIRECTION_UP;
            display0_on_timer1_trigger();
            timer1_task_500ms_counter ++;
            timer1_enable();
            break;
        }
    }
}

void display0_reset()
{
    display0.sec = 0;
    display0.min = 0;
    display0.colon = 0;
    display0.state = DISPLAY_COUNT_STOP;
    display_send_data(&display0);
    led_off(&led0);
}

void display0_on_time_top()
{
    led_on(&led0);
}

void display0_on_time_zero()
{
    led_on(&led0);
}
