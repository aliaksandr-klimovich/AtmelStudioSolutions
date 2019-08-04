/*
 * led0.c
 *
 * Created: 04.08.2019 18:38:16
 *  Author: Aliaksandr
 */ 

#include "led0.h"

inline void led0_configure()
{
    DDR_SET(LED13);
    PORT_CLEAR(LED13);
}

void led0_on()
{
    PORT_SET(LED13);
}

void led0_off()
{
    PORT_CLEAR(LED13);
}

void led0_toggle()
{
    PIN_SET(LED13);
}
