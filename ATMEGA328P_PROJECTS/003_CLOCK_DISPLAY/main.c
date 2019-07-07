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
    OCR1AL = 0x71;  // 625
    TIMSK1 |= (1 << OCIE1A);
}

void enable_timer1()
{
    // set clock source
    TCCR1B |= (1 << CS10) | (1 << CS12);  // 1024
}

#define TIMER1_TOP  (16000000UL / 1024UL / 625UL)

uint8_t timer1_counter = 0;
void task_200ms();
void task_500ms();
void task_1s();
uint8_t task_1s_cycles = 0;
uint8_t task_200ms_cycles = 0;
uint8_t task_500ms_cycles = 0;

ISR(TIMER1_COMPA_vect)
{
    timer1_counter++;

    if (timer1_counter % (TIMER1_TOP / 5) == 0)
    {
        task_200ms();
        task_200ms_cycles++;
    }
    
    if (timer1_counter % (TIMER1_TOP / 2) == 0)
    {
        task_500ms();
        task_500ms_cycles++;
    }

    if (timer1_counter == TIMER1_TOP)
    {
        task_1s();
        task_1s_cycles++;
        timer1_counter = 0;
    }
}

void task_200ms()
{

}

uint8_t min = 0;
uint8_t sec = 0;

void task_500ms()
{
    if (task_500ms_cycles % 2)
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

void task_1s()
{
    
}

int main(void)
{    
    TM1637_init(&PC0, &PC1);

    sleep_enable();
    configure_timer1();
    enable_timer1();
    sei();

    /*
    #include <avr/boot.h>
    uint8_t low_fuses = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);     // 255 = 1111_1111
    uint8_t high_fuses = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);   // 218 = 1101_1010
    TM1637_print("%u", high_fuses);

    low_fuses
        7 CKDIV8 = 1
        6 CKOUT = 1
        5 SUT1 = 1
        4 SUT0 = 1
        3 CKSEL3 = 1
        2 CKSEL2 = 1
        1 CKSEL1 = 1
        0 CKSEL0 = 1

    high_fuses
        7 RSTDISBL = 1
        6 DWEN = 1
        5 SPIEN = 0
        4 WDTON = 1
        3 EESAVE = 1
        2 BOOTSZ1 = 0
        1 BOOTSZ0 = 1
        0 BOOTRST = 0
    */

    while (1)
    {
        sleep_cpu();
    }
}

