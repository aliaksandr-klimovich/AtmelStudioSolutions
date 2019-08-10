/*
 * main.h
 *
 * Created: 10.08.2019 13:00:17
 *  Author: Aliaksandr
 */ 

#ifndef MAIN_H_
#define MAIN_H_

#define OSCILLATOR_FREQUENCY 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "buzzer/buzzer.h"
extern Buzzer buzzer0;

#include "led/led.h"
#include "pin_map/avr_map.h"
#define led0 PB5

#include "button/button.h"
extern Button button0;

#include "display/display.h"
extern Display display0; 

#define TIMER1_COUNTER_DIVIDER 125U
#include "tasking/tasking.h"

#endif /* MAIN_H_ */
