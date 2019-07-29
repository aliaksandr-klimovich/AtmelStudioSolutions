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
#include "PIN_MAP/avr_map.h"  // overrides <avr/io.h> pin definitions


// --- timer1 variables  ---

#define OSCILLATOR_FREQUENCY 16000000UL
#define TIMER1_CLK_IO_DIVIDER 1024UL
const uint8_t TIMER1_COUNTER_TOP_VALUE = OSCILLATOR_FREQUENCY / TIMER1_CLK_IO_DIVIDER / 625UL;  // 625 should be set in `timer1_configure` function

uint8_t timer1_counter = 0;

inline void timer1_configure();
inline void timer1_enable();
inline void int0_configure();


// --- tasking variables ---

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
uint8_t task_500ms_counter = 0;

void task_40ms();
void task_200ms();
void task_500ms();
void task_1s();


// --  led13 variables ---

#define LED13 PB5

inline void led13_configure();
void led13_on();
void led13_off();
void led13_toggle();


// --- button0 variables ---

const uint8_t BUTTON0_LONG_PRESS_COUNTER_TOP_VALUE = 15;  // 15 * 40ms = 600ms

typedef enum ButtonSwitch_t
{
    KEY_UNDEFINED,
    KEY_UP,
    KEY_DOWN,
    KEY_PRESS,
} ButtonSwitch;

ButtonSwitch button0_switch = KEY_UNDEFINED;
uint8_t button0_press_counter = 0;

inline void button0_configure();
inline void button0_press_short();
inline void button0_press_long();
inline void button0_key_down();
inline void button0_key_up();
inline void button0_handler();


// --- display variables ---

typedef enum DisplayState_t
{
    DISPLAY_COUNT_STOP,
    DISPLAY_COUNT_START,
    DISPLAY_COUNT_DIRECTION_UP,
    DISPLAY_COUNT_DIRECTION_DOWN,
    DISPLAY_COUNT_RESET,
} DisplayState;

DisplayState display_state = DISPLAY_COUNT_STOP;
int8_t display_min = 0;
int8_t display_sec = 0;

void display_print(uint8_t colon);
void display_time_out();
void display_reset();
inline void display_handler();
inline void display_start();


// --- led13 functions ---

void led13_on()
{
    PORT_SET(LED13);
}

void led13_off()
{
    PORT_CLEAR(LED13);
}

void led13_toggle()
{
    PIN_SET(LED13);
}


// --- timer1 functions  ---

inline void timer1_configure()
{
    TCCR1B |= (1<< WGM12);  // set CTC mode
    OCR1AH = 0x02;
    OCR1AL = 0x71;  // 625
    TIMSK1 |= (1 << OCIE1A);
}

inline void timer1_enable()
{
    // set clock source
    TCCR1B |= (1 << CS10) | (1 << CS12);  // 1024
}


//  int0 functions

inline void int0_configure()
{
    EICRA |= (1 << ISC00); // any logical change on INT0 generates an interrupt request
    EIMSK |= (1 << INT0);  // enable INT0
}


// --- button0 functions ---

inline void button0_configure()
{
    DDR_CLEAR(PD2);  // PD2 as input
    PORT_SET(PD2);  // enable pull up resistor
}

inline void button0_press_short()
{
    switch (display_state)
    {
        case DISPLAY_COUNT_DIRECTION_UP:
            display_state = DISPLAY_COUNT_DIRECTION_DOWN;
            break;
        case DISPLAY_COUNT_DIRECTION_DOWN:
            display_state = DISPLAY_COUNT_DIRECTION_UP;
            break;
        case DISPLAY_COUNT_STOP:
            display_state = DISPLAY_COUNT_START;
            break;
        case DISPLAY_COUNT_START:
        case DISPLAY_COUNT_RESET:
            break;
    }
}

inline void button0_press_long()
{
    display_state = DISPLAY_COUNT_RESET;
}

inline void button0_key_down()
{

}

inline void button0_key_up()
{

}

inline void button0_handler()
{
    switch (button0_switch)
    {
        case KEY_UNDEFINED:
            break;

        case KEY_DOWN:
            button0_key_down();
            button0_press_counter = 0;
            button0_switch = KEY_PRESS;
            break;

        case KEY_UP:
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

        case KEY_PRESS:
            button0_press_counter++;
            if (button0_press_counter == 255)
            {
                button0_press_counter = BUTTON0_LONG_PRESS_COUNTER_TOP_VALUE;  
            }
            break;
    }
}


// --- led13 functions---

inline void led13_configure()
{
    DDR_SET(LED13);
    PORT_CLEAR(LED13);
}


//  --- display functions ---

void display_print(uint8_t colon)
{
    if (colon)
    {
        TM1637_print("%02u:%02u", display_min, display_sec);
    }
    else
    {
        TM1637_print("%02u %02u", display_min, display_sec);
    }
}

inline void display_time_out()
{
    led13_on();
}

inline void display_start()
{
    led13_off();
}

inline void display_reset()
{

}

inline void display_handler()
{
    switch (display_state)
    {
        case DISPLAY_COUNT_STOP:
            break;
        
        case DISPLAY_COUNT_START:
            display_start();
            display_state = DISPLAY_COUNT_DIRECTION_UP;
            // no break;   

        case DISPLAY_COUNT_DIRECTION_UP:
            if (task_500ms_counter % 2)
            {
                display_print(0);
                if (++display_sec >= 60)
                {
                    display_sec = 0;
                    if (++display_min >= 60)
                    {
                        display_min = 0;
                    }
                }
            }
            else
            {
                display_print(1);
            }
            break;

        case DISPLAY_COUNT_DIRECTION_DOWN:
            if (task_500ms_counter % 2)
            {
                display_print(0);
                if (--display_sec < 0)
                {
                    display_sec = 59;
                    if (--display_min < 0)
                    {
                        display_min = 0;
                        display_sec = 0;
                        display_state = DISPLAY_COUNT_STOP;
                        display_time_out();
                    }
                }
            }
            else
            {
                display_print(1);
            }
            break;

        case DISPLAY_COUNT_RESET:
            if (task_500ms_counter % 2)
            {
                display_min = 0;
                display_sec = 0;
                display_state = DISPLAY_COUNT_START;
                display_reset();
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

    if (timer1_counter % (TIMER1_COUNTER_TOP_VALUE / 25) == 0)
    {
        task_switch.b.task_40ms_switch = 1;
    }

    if (timer1_counter % (TIMER1_COUNTER_TOP_VALUE / 5) == 0)
    {
        task_switch.b.task_200ms_switch = 1;
    }

    if (timer1_counter % (TIMER1_COUNTER_TOP_VALUE / 2) == 0)
    {
        task_switch.b.task_500ms_switch = 1;
    }

    if (timer1_counter == TIMER1_COUNTER_TOP_VALUE)
    {
        task_switch.b.task_1s_switch = 1;
        timer1_counter = 0;
    }
}

ISR(INT0_vect, ISR_NOBLOCK)
{
    if (!PIN_CHECK(PD2))
    {
        switch (button0_switch)
        {
            case KEY_UNDEFINED:
                button0_switch = KEY_DOWN;
                break;
            case KEY_UP:
            case KEY_DOWN:
            case KEY_PRESS:
                break;
        }
    }
    else
    {
        switch (button0_switch)
        {
            case KEY_UNDEFINED:
            case KEY_UP:
                break;
            case KEY_DOWN:  // handler for button0 was not executed
                button0_switch = KEY_UNDEFINED;
                break;
            case KEY_PRESS:
                button0_switch = KEY_UP;
                break;
        }
    }
}


//  --- ===== ***  MAIN  *** ===== ---

int main(void)
{
    cli();

    TM1637_init(&PC0, &PC1);

    int0_configure();
    button0_configure();

    led13_configure();

    timer1_configure();
    timer1_enable();

    // small hard code
    // 10 means 10 asm instructions, see .lss file
//     for (uint32_t i = 0; i != OSCILLATOR_FREQUENCY / 10; i++)  // 1s
//     {
//         asm("NOP");
//     }

    sleep_enable();
    sei();
    sleep_cpu();

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
