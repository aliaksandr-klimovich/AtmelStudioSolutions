#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdint.h>

#include "../pin_map/pin_type.h"


typedef enum ButtonState_t
{
    BUTTON_KEY_UNDEFINED,
    BUTTON_KEY_UP,
    BUTTON_KEY_DOWN,
    BUTTON_KEY_PRESS,
} ButtonState;

typedef struct Button_t  
{
    PIN *dio;
    ButtonState state;
    uint8_t _press_counter;
    uint8_t _long_press_top_value; 
} Button; 

void button_handler(Button *button);
void button_on_key_down(Button *button);
void button_on_key_up(Button *button);
void button_on_press_short(Button *button);
void button_on_press_long(Button *button);

void button0_init();

#endif /* BUTTON_H_ */