#ifndef LED0_H_
#define LED0_H_

#include "PIN_MAP/avr_map.h"

#define LED13 PB5

void led0_configure();
void led0_on();
void led0_off();
void led0_toggle();

#endif /* LED0_H_ */