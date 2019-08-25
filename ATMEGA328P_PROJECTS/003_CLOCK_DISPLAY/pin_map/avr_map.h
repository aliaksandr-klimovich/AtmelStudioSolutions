/*

  AVR pin mapping

This module re-defines standard port mapping (aka PB1, PD3...) to PIN (struct) mapping.

*/

#ifndef AVR_MAP_H_
#define AVR_MAP_H_

#include "pin_type.h"

#ifdef PB2
#undef PB2
#endif
extern PIN PB2;

#ifdef PB5
#undef PB5
#endif
extern PIN PB5;

#ifdef PC0
#undef PC0
#endif
extern PIN PC0;

#ifdef PC1
#undef PC1
#endif
extern PIN PC1;

#ifdef PD2
#undef PD2
#endif
extern PIN PD2;

#ifdef PD3
#undef PD3
#endif
extern PIN PD3;

#ifdef PD6
#undef PD6
#endif
extern PIN PD6;

#endif /* AVR_MAP_H_ */
