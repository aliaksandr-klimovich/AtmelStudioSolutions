/*
 * 003_CLOCK_DISPLAY.c
 *
 * Created: 14.06.2019 22:02:55
 * Author : Aliaksandr
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "TM1637/TM1637.h"
#include "PIN_MAP/pin_map.h"
#include "PIN_MAP/avr_map.h"  // overrides <avr/io.h> pins


// --- tasking variables ---

const uint8_t TIMER1_TOP = 16000000UL / 1024UL / 625UL;  // 625 should be set in configure_timer0() function
uint8_t timer1_counter = 0;
uint8_t task_500ms_counter = 0;

typedef union TaskSwitch_t
{
    struct
    {
        uint8_t task_1s_switch: 1;
        uint8_t task_500ms_switch: 1;
        uint8_t task_200ms_switch: 1;
        uint8_t task_40ms_switch: 1;
        uint8_t bit4: 1;
        uint8_t bit5: 1;
        uint8_t bit6: 1;
        uint8_t bit7: 1;
    } b;
    uint8_t u8;
} TaskSwitch;

TaskSwitch task_switch;


// --  led13 variables ---

#define LED13 PB5


// --- button0 variables ---

typedef enum ButtonSwitch_t
{
    KEY_UNDEFINED,
    KEY_UP,
    KEY_DOWN,
} ButtonSwitch;

ButtonSwitch button0_switch = KEY_UNDEFINED;


// --- timer0 variables ---

int8_t min = 0;
int8_t sec = 0;

typedef enum CountDirection
{
    COUNT_DIRECTION_UP,
    COUNT_DIRECTION_DOWN,
    COUNT_NO_DIRECTION,
} CountDirection;

CountDirection count_direction = COUNT_DIRECTION_UP;


// --- led13 functions ---

void led13_on()
{
    SET_PORT(LED13);
}

void led13_off()
{
    CLEAR_PORT(LED13);
}

void led13_toggle()
{
    SET_PIN(LED13);
}


// --- timer0 functions  ---

inline void configure_timer1()
{
    TCCR1B |= (1<< WGM12);  // set CTC mode
    OCR1AH = 0x02;
    OCR1AL = 0x71 - 3;  // 625 +/- adj.: (+) slower (-) faster
    TIMSK1 |= (1 << OCIE1A);
}

inline void enable_timer1()
{
    // set clock source
    TCCR1B |= (1 << CS10) | (1 << CS12);  // 1024
}


// --- button0 functions ---

inline void configure_int0()
{
    EICRA |= (1 << ISC00); // | (1 << ISC00);  // any logical change on INT0 generates an interrupt request
    EIMSK |= (1 << INT0);  // enable INT0
}

inline void configure_button0()
{
    // PD2 as input, pull up
    CLEAR_DDR(PD2);
    SET_PORT(PD2);
}

inline void button0_on_key_down()
{
    //led13_toggle();
    if (count_direction == COUNT_DIRECTION_UP)
        count_direction = COUNT_DIRECTION_DOWN;
    //else if (count_direction == COUNT_DIRECTION_DOWN)
    //    count_direction = COUNT_DIRECTION_UP;
    else
        count_direction = COUNT_DIRECTION_UP;
}

inline void button0_on_key_up()
{
    //led13_toggle();
}

inline void button0_handler()
{
    switch (button0_switch)
    {
        case KEY_UNDEFINED:
            break;

        case KEY_DOWN:
            button0_on_key_down();
            break;

        case KEY_UP:
            button0_on_key_up();
            break;
    }

    button0_switch = KEY_UNDEFINED;
}


// --- led13 functions---

void configure_led13()
{
    SET_DDR(LED13);
    CLEAR_PORT(LED13);
}

//  --- display functions ---

void display_print(uint8_t colon)
{
    if (colon)
        TM1637_print("%02u:%02u", min, sec);
    else
        TM1637_print("%02u %02u", min, sec);
}

void display_on_time_out()
{
    led13_on();
}

inline void display_handler()
{
    switch (count_direction)
    {
        case COUNT_NO_DIRECTION:
            break;

        case COUNT_DIRECTION_UP:
            if (task_500ms_counter % 2)
            {
                display_print(0);
                if (++sec >= 60)
                {
                    sec = 0;
                    if (++min >= 60)
                    {
                        min = 0;
                        //count_direction = COUNT_NO_DIRECTION;
                        //display_on_time_out();
                    }
                }
            }
            else
            {
                display_print(1);
            }
            break;

        case COUNT_DIRECTION_DOWN:
            if (task_500ms_counter % 2)
            {
                display_print(0);
                if (--sec < 0)
                {
                    sec = 59;
                    if (--min < 0)
                    {
                        min = 0;
                        sec = 0;
                        count_direction = COUNT_NO_DIRECTION;
                        display_on_time_out();
                    }
                }
            }
            else
            {
                display_print(1);
            }
            break;
    }
}


// --- tasking functions ---

inline void task_40ms()
{
    button0_handler();
}

inline void task_200ms()
{

}

inline void task_500ms()
{
    display_handler();
}

inline void task_1s()
{

}


// --- interrupt service routines ---

ISR(TIMER1_COMPA_vect)
{
    timer1_counter++;

    if (timer1_counter % (TIMER1_TOP / 25) == 0)
    {
        task_switch.b.task_40ms_switch = 1;
    }

    if (timer1_counter % (TIMER1_TOP / 5) == 0)
    {
        task_switch.b.task_200ms_switch = 1;
    }

    if (timer1_counter % (TIMER1_TOP / 2) == 0)
    {
        task_switch.b.task_500ms_switch = 1;
    }

    if (timer1_counter == TIMER1_TOP)
    {
        task_switch.b.task_1s_switch = 1;
        timer1_counter = 0;
    }
}

ISR(INT0_vect, ISR_NOBLOCK)
{
    if (!CHECK_PIN(PD2))
    {
        button0_switch = KEY_DOWN;
    }
    else
    {
        button0_switch = KEY_UP;
    }
}


//  --- ==== *** MAIN *** ==== ---

int main(void)
{
    sleep_enable();

    TM1637_init(&PC0, &PC1);

    configure_int0();
    configure_button0();

    configure_led13();

    configure_timer1();
    enable_timer1();

    sei();

    while (1)
    {
        if (task_switch.b.task_40ms_switch)
        {
            task_40ms();
            task_switch.b.task_40ms_switch = 0;
        }

        if (task_switch.b.task_200ms_switch)
        {
            task_200ms();
            task_switch.b.task_200ms_switch = 0;
        }


        if (task_switch.b.task_500ms_switch)
        {
            task_500ms();
            task_500ms_counter++;
            task_switch.b.task_500ms_switch = 0;
        }

        if (task_switch.b.task_1s_switch)
        {
            task_1s();
            task_switch.b.task_1s_switch = 0;
        }

        sleep_cpu();
    }
}
