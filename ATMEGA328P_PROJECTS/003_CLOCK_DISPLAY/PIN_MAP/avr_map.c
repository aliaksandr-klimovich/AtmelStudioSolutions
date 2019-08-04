/*
 * avr_map.c
 *
 * Created: 04.08.2019 17:30:12
 *  Author: Aliaksandr
 */ 

#include "avr_map.h"

PIN PB2 = {&DDRB, &PORTB, &PINB, 2};
PIN PB5 = {&DDRB, &PORTB, &PINB, 5};
    
PIN PC0 = {&DDRC, &PORTC, &PINC, 0};
PIN PC1 = {&DDRC, &PORTC, &PINC, 1};
    
PIN PD2 = {&DDRD, &PORTD, &PIND, 2};
        