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

void configure_timer1()
{
    TCCR1B |= (1<< WGM12);  // set CTC mode
    OCR1AH = 0x02;
    OCR1AL = 0x71 - 3;  // 625 +/- adj.: (+) slower (-) faster
    TIMSK1 |= (1 << OCIE1A);
}

void enable_timer1()
{
    // set clock source
    TCCR1B |= (1 << CS10) | (1 << CS12);  // 1024
}

#define TIMER1_TOP  (16000000UL / 1024UL / 625UL)

uint8_t timer1_counter;

void task_1s();
void task_500ms();
void task_200ms();

uint8_t task_1s_counter;
uint8_t task_500ms_counter;
uint8_t task_200ms_counter;

union TaskSwitches {
    struct {
        uint8_t task_1s_switch: 1;
        uint8_t task_500ms_switch: 1;
        uint8_t task_200ms_switch: 1;
        uint8_t bit3: 1;
        uint8_t bit4: 1;
        uint8_t bit5: 1;
        uint8_t bit6: 1;
        uint8_t bit7: 1;
    } b;
    uint8_t u8;
} task_switches;

ISR(TIMER1_COMPA_vect)
{
    timer1_counter++;

    if (timer1_counter % (TIMER1_TOP / 5) == 0)
    {
        //task_200ms();
        task_switches.b.task_200ms_switch = 1;
    }

    if (timer1_counter % (TIMER1_TOP / 2) == 0)
    {
        //task_500ms();
        task_switches.b.task_500ms_switch = 1;
    }

    if (timer1_counter == TIMER1_TOP)
    {
        //task_1s();
        task_switches.b.task_1s_switch = 1;
        timer1_counter = 0;
    }
}

void task_1s()
{

}

uint8_t min;
uint8_t sec;

void task_500ms()
{
    if (task_500ms_counter % 2)
    {
        TM1637_print("%02u %02u", min, sec);

        if (++sec >= 60)
        {
            sec = 0;
            if (++min >= 60)
            {
                min = 0;
            }
        }
    }
    else
    {
        TM1637_print("%02u:%02u", min, sec);
    }
}

void task_200ms()
{

}

ISR(INT0_vect)
{
    
}

int main(void)
{
    TM1637_init(&PC0, &PC1);

    sleep_enable();
    configure_timer1();
    enable_timer1();
    sei();

    while (1)
    {
        /*if (task_switches.b.task_1s_switch)
        {
            task_1s();
            task_1s_counter++;
            task_switches.b.task_1s_switch = 0;
        }*/
        if (task_switches.b.task_500ms_switch)
        {
            task_500ms();
            task_500ms_counter++;
            task_switches.b.task_500ms_switch = 0;
        }
        /*if (task_switches.b.task_200ms_switch)
        {
            task_200ms();
            task_200ms_counter++;
            task_switches.b.task_200ms_switch = 0;
        }*/
        sleep_cpu();
    }
}
