#ifndef BUZZER_H_
#define BUZZER_H_

#include <stdbool.h>

#include "../pin_map/pin_type.h"

typedef struct Buzzer_t
{
    PIN *dio;
    bool enabled;
} Buzzer;

void buzzer0_init();
void buzzer0_disable();
void buzzer0_enable();
void buzzer0_trigger();

#endif /* BUZZER_H_ */
