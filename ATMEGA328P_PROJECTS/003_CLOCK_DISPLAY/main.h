#ifndef MAIN_H_
#define MAIN_H_

#define OSCILLATOR_FREQUENCY 16000000UL

#include "buzzer/buzzer.h"
extern Buzzer buzzer0;

#include "led/led.h"
#include <avr/io.h>
#include "pin_map/avr_map.h"
#define led0 PB5

#include "button/button.h"
extern Button button0;

#include "display/display.h"
extern Display display0;

#include "timer1/timer1.h"

#endif /* MAIN_H_ */
