#include <avr/io.h>

#include "buzzer.h"
#include "../main.h"

void buzzer_init(Buzzer *buzzer)
{
    PIN_AS_OUTPUT_HIGH(buzzer->dio);
    buzzer->enabled = false;
    buzzer->state = BUZZER_STOP;
    buzzer->counter = 0;
    buzzer->tone = BUZZER_TONE_1;
}

inline void buzzer_enable(Buzzer *buzzer)
{
    buzzer->enabled = true;
}

inline void buzzer_disable(Buzzer *buzzer)
{
    buzzer->enabled = false;
}

inline void buzzer_trigger(Buzzer *buzzer)
{
    buzzer->enabled ? buzzer_disable(buzzer) : buzzer_enable(buzzer);
}

void buzzer0_init()
{
    // Default configuration for buzzer
    buzzer_init(&buzzer0);

    buzzer0_disable();

    // Configure timer2 for buzzer0
    // Set clear timer on compare match (CTC) mode 
    TCCR2A = (1 << WGM21);

    // Configure tone 
    buzzer0_change_tone(BUZZER_TONE_1);
    TCCR2B = (1 << CS22);   // CLK/128 (from prescaler) 
}

void buzzer0_disable()
{
    // Disconnect output compare unit 
    TCCR2A &= ~(1 << COM2B0);
    buzzer_disable(&buzzer0);
}

void buzzer0_enable()
{
    buzzer0_change_tone();
        
    // Toggle OC2A on compare match 
    TCCR2A |= (1 << COM2B0);
    buzzer_enable(&buzzer0);
}

void buzzer0_handler()
{
    switch (buzzer0.state)
    {
        case BUZZER_STOP:
            break;
        case BUZZER_1_SHORT_CLICK:
            if (buzzer0.counter == 0)  // count did not start yet
            {   
                buzzer0_enable();
            }                
            else if (buzzer0.counter >= (80 / TIMER1_TASK_TICK))  
            {
                buzzer0_disable();
                buzzer0.counter = 0;
                buzzer0.state = BUZZER_STOP;
                break;
            }
            else  // is counting
            {
                // do nothing
            }
            buzzer0.counter ++;
            break;
        case BUZZER_1_LONG_CLICK:
            if (buzzer0.counter == 0)
            {
                buzzer0_enable();
            }
            else if (buzzer0.counter >= (800 / TIMER1_TASK_TICK))
            {
                buzzer0_disable();
                buzzer0.counter = 0;
                buzzer0.state = BUZZER_STOP;
                break;
            }
            else  // is counting
            {
                // do nothing
            }
            buzzer0.counter ++;
            break;
        case BUZZER_3_SHORT_CLICKS:
            #define BUZZER_3_SHORT_CLICKS_DUTY_CYCLE 80
            #define BUZZER_3_SHORT_CLICKS_NUMBER_OF_CLICKS 3
            
            if (buzzer0.counter >= (BUZZER_3_SHORT_CLICKS_DUTY_CYCLE * 
                (BUZZER_3_SHORT_CLICKS_NUMBER_OF_CLICKS * 2 - 1) / TIMER1_TASK_TICK))
            {
                buzzer0_disable();
                buzzer0.counter = 0;
                buzzer0.state = BUZZER_STOP;
                break;  
            }
            else if ((buzzer0.counter % (BUZZER_3_SHORT_CLICKS_DUTY_CYCLE * 2 / TIMER1_TASK_TICK)) == 0)
            {
                buzzer0_enable();
            }
            else if (((buzzer0.counter + (BUZZER_3_SHORT_CLICKS_DUTY_CYCLE / TIMER1_TASK_TICK)) % 
                (BUZZER_3_SHORT_CLICKS_DUTY_CYCLE * 2 / TIMER1_TASK_TICK)) == 0)
            {
                buzzer0_disable();
            }
            else  // count
            {
                // do nothing
            }
            buzzer0.counter ++;
            break;
    }
} 

void buzzer0_1_short_click()
{
    if (buzzer0.state == BUZZER_STOP)
    {
        buzzer0.tone = BUZZER_TONE_2;
        buzzer0.state = BUZZER_1_SHORT_CLICK;
    }
}

void buzzer0_1_long_click()
{
    if (buzzer0.state == BUZZER_STOP)
    {
        buzzer0.tone = BUZZER_TONE_2;
        buzzer0.state = BUZZER_1_LONG_CLICK;
    }   
}

void buzzer0_change_tone()
{
    OCR2A = buzzer0.tone;  // Timer2 output compare match register
}

void buzzer0_3_short_clicks()
{
    if (buzzer0.state == BUZZER_STOP)
    {
        buzzer0.tone = BUZZER_TONE_2;
        buzzer0.state = BUZZER_3_SHORT_CLICKS;
    }
} 