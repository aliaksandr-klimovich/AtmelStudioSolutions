#ifndef BUZZER_H_
#define BUZZER_H_

// Internal dependencies
#include <stdint.h>
#include <avr/io.h>
#include "../pin_map/pin_map.h"

typedef struct Buzzer_t 
{
    PIN *dio;
    uint8_t enabled;
} Buzzer;

void buzzer_init(Buzzer *buzzer);
void buzzer_disable(Buzzer *buzzer);
void buzzer_enable(Buzzer *buzzer);
void buzzer_trigger(Buzzer *buzzer);

#endif /* BUZZER_H_ */