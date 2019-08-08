#ifndef BUTTON0_H_
#define BUTTON0_H_

#include <stdint.h>
#include "PIN_MAP/avr_map.h"
#include "display0.h"
#include "buzzer0.h"

typedef enum ButtonSwitch_t
{
    KEY_UNDEFINED,
    KEY_UP,
    KEY_DOWN,
    KEY_PRESS,
} ButtonSwitch;

extern ButtonSwitch button0_switch;

void button0_configure();
void button0_press_short();
void button0_press_long();
void button0_key_down();
void button0_key_up();
void button0_handler();

#endif /* BUTTON0_H_ */