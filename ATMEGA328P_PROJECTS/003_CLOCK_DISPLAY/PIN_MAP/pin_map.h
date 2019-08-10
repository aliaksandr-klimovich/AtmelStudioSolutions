#ifndef PIN_MAP_H_
#define PIN_MAP_H_

#include <stdint.h>

typedef const struct {
    volatile uint8_t *DDR_ADDR;
    volatile uint8_t *PORTR_ADDR;
    volatile uint8_t *PINR_ADDR;
    uint8_t ID;
} PIN;

#define DDR_SET_P(pin)   (*((pin)->DDR_ADDR)   |= (1 << (pin)->ID))
#define PORTR_SET_P(pin) (*((pin)->PORTR_ADDR) |= (1 << (pin)->ID))
#define PINR_SET_P(pin)  (*((pin)->PINR_ADDR)  |= (1 << (pin)->ID))

#define DDR_CLEAR_P(pin)   (*((pin)->DDR_ADDR)   &= ~(1 << (pin)->ID))
#define PORTR_CLEAR_P(pin) (*((pin)->PORTR_ADDR) &= ~(1 << (pin)->ID))
#define PIN_CLEAR_P(pin)   (*((pin)->PINR_ADDR)  &= ~(1 << (pin)->ID))

#define PORTR_CHECK_P(pin) (*(pin)->PORTR_ADDR & (1 << (pin)->ID))
#define PINR_CHECK_P(pin)  (*(pin)->PINR_ADDR  & (1 << (pin)->ID))

#define PORTR_READ_P(pin) (PORTR_CHECK_P(pin) ? 1 : 0)
#define PINR_READ_P(pin)  (PINR_CHECK_P(pin)  ? 1 : 0)

#define DDR_SET(pin)   (*((pin).DDR_ADDR)   |= (1 << (pin).ID))
#define PORTR_SET(pin) (*((pin).PORTR_ADDR) |= (1 << (pin).ID))
#define PINR_SET(pin)  (*((pin).PINR_ADDR)  |= (1 << (pin).ID))

#define DDR_CLEAR(pin)   (*((pin).DDR_ADDR)   &= ~(1 << (pin).ID))
#define PORTR_CLEAR(pin) (*((pin).PORTR_ADDR) &= ~(1 << (pin).ID))
#define PINR_CLEAR(pin)  (*((pin).PINR_ADDR)  &= ~(1 << (pin).ID))

#define PORTR_CHECK(pin) (*((pin).PORTR_ADDR) & (1 << (pin).ID))
#define PINR_CHECK(pin)  (*((pin).PINR_ADDR)  & (1 << (pin).ID))

#define PORTR_READ(pin) (PORTR_CHECK(pin) ? 1 : 0)
#define PINR_READ(pin)  (PINR_CHECK(pin)  ? 1 : 0)

// todo: make macro to set pin as input, output, pull up...

#endif /* PIN_MAP_H_ */
