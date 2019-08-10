#ifndef LED_H_
#define LED_H_

// Internal dependencies
#include "../pin_map/pin_map.h"

#define led_init(pin) \
do { \
    DDR_SET_P(pin); \
    PORTR_CLEAR_P(pin); \
} while(0);

#define led_on(pin) PORTR_SET_P(pin)
#define led_off(pin) PORTR_CLEAR_P(pin)
#define led_toggle(pin) PINR_SET_P(pin)

#endif /* LED_H_ */