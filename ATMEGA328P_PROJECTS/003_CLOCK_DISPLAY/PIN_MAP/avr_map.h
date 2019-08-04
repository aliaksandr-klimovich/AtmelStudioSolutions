/*
 * avr_map.h
 *
 * Created: 07.07.2019 12:02:44
 *  Author: Aliaksandr
 */


#ifndef AVR_MAP_H_
#define AVR_MAP_H_

/*
 * AVR stuff
 *
 */

#include <avr/io.h>
#include "pin_map.h"

#ifdef PB2
#undef PB2
#endif
PIN PB2;

#ifdef PB5
#undef PB5
#endif
PIN PB5;

#ifdef PC0
#undef PC0
#endif
PIN PC0;

#ifdef PC1
#undef PC1
#endif
PIN PC1;
    
#ifdef PD2
#undef PD2
#endif
PIN PD2;

#endif /* AVR_MAP_H_ */
