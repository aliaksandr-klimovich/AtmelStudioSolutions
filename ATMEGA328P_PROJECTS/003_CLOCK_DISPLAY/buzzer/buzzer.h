#ifndef BUZZER_H_
#define BUZZER_H_

#include <stdbool.h>

#include "../pin_map/pin_type.h"

typedef enum BuzzerState_t
{
    BUZZER_STOP,
    BUZZER_1_SHORT_CLICK,
    BUZZER_1_LONG_CLICK,
} BuzzerState;

typedef enum BuzzerTone_t
{
    BUZZER_TONE_1 = 0x24,
    BUZZER_TONE_2 = 0x3A,
} BuzzerTone;

typedef struct Buzzer_t
{
    PIN *dio;
    bool enabled;
    BuzzerState state;
    uint8_t counter;  // counts one click time
    uint8_t tone;
} Buzzer;

void buzzer_init(Buzzer *buzzer);
void buzzer_trigger(Buzzer *buzzer);
void buzzer_disable(Buzzer *buzzer);
void buzzer_enable(Buzzer *buzzer);

void buzzer0_init();
void buzzer0_disable();
void buzzer0_enable();
void buzzer0_handler();
void buzzer0_1_short_click();
void buzzer0_1_long_click();
void buzzer0_change_tone();

#endif /* BUZZER_H_ */
