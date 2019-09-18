/*

  AVR pin mapping
  
  This module re-defines standard port mapping (aka PB1, PD3...) to PIN (struct) mapping.
  The PIN struct allows to unite DDR, PORT, PIN register and also the pin number.
  To use the structures below, include avr_map.h file in your code.

  Note, this is a generated file.
  
*/

#include <avr/io.h>
#include "avr_map.h"

PIN PB0 = {&DDRB, &PORTB, &PINB, 0};
PIN PB1 = {&DDRB, &PORTB, &PINB, 1};
PIN PB2 = {&DDRB, &PORTB, &PINB, 2};
PIN PB3 = {&DDRB, &PORTB, &PINB, 3};
PIN PB4 = {&DDRB, &PORTB, &PINB, 4};
PIN PB5 = {&DDRB, &PORTB, &PINB, 5};
PIN PB6 = {&DDRB, &PORTB, &PINB, 6};
PIN PB7 = {&DDRB, &PORTB, &PINB, 7};

PIN PC0 = {&DDRC, &PORTC, &PINC, 0};
PIN PC1 = {&DDRC, &PORTC, &PINC, 1};
PIN PC2 = {&DDRC, &PORTC, &PINC, 2};
PIN PC3 = {&DDRC, &PORTC, &PINC, 3};
PIN PC4 = {&DDRC, &PORTC, &PINC, 4};
PIN PC5 = {&DDRC, &PORTC, &PINC, 5};
PIN PC6 = {&DDRC, &PORTC, &PINC, 6};
PIN PC7 = {&DDRC, &PORTC, &PINC, 7};

PIN PD0 = {&DDRD, &PORTD, &PIND, 0};
PIN PD1 = {&DDRD, &PORTD, &PIND, 1};
PIN PD2 = {&DDRD, &PORTD, &PIND, 2};
PIN PD3 = {&DDRD, &PORTD, &PIND, 3};
PIN PD4 = {&DDRD, &PORTD, &PIND, 4};
PIN PD5 = {&DDRD, &PORTD, &PIND, 5};
PIN PD6 = {&DDRD, &PORTD, &PIND, 6};
PIN PD7 = {&DDRD, &PORTD, &PIND, 7};
