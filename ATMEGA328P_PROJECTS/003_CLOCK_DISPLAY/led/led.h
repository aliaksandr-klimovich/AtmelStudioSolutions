#ifndef LED_H_
#define LED_H_

#include "../pin_map/pin_type.h"

#define led_init(pin)       \
do {                        \
    PIN_AS_OUTPUT_LOW(pin); \
} while(0)

#define led_on(pin) PORTR_SET(pin)
#define led_off(pin) PORTR_CLEAR(pin)
#define led_toggle(pin) PINR_SET(pin)

#endif /* LED_H_ */
