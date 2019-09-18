#ifndef PIN_MAP_H_
#define PIN_MAP_H_

#include <stdint.h>

typedef const struct {
    volatile uint8_t *DDR_ADDR;
    volatile uint8_t *PORTR_ADDR;
    volatile uint8_t *PINR_ADDR;
    uint8_t ID;
} PIN;

#define DDR_SET(pin)   (*((pin)->DDR_ADDR)   |= (1 << (pin)->ID))
#define PORTR_SET(pin) (*((pin)->PORTR_ADDR) |= (1 << (pin)->ID))
#define PINR_SET(pin)  (*((pin)->PINR_ADDR)  |= (1 << (pin)->ID))

#define DDR_CLEAR(pin)   (*((pin)->DDR_ADDR)   &= ~(1 << (pin)->ID))
#define PORTR_CLEAR(pin) (*((pin)->PORTR_ADDR) &= ~(1 << (pin)->ID))
#define PIN_CLEAR(pin)   (*((pin)->PINR_ADDR)  &= ~(1 << (pin)->ID))

#define PORTR_CHECK(pin) (*((pin)->PORTR_ADDR) & (1 << (pin)->ID))
#define PINR_CHECK(pin)  (*((pin)->PINR_ADDR)  & (1 << (pin)->ID))

#define PORTR_READ(pin) (PORTR_CHECK(pin) ? 1 : 0)
#define PINR_READ(pin)  (PINR_CHECK(pin)  ? 1 : 0)

#define PIN_AS_INPUT_PULLUP(pin)    \
do {                                \
    DDR_CLEAR(pin);                 \
    PORTR_SET(pin);                 \
} while(0)

#define PIN_AS_INPUT_NO_PULLUP(pin) \
do {                                \
    DDR_CLEAR(pin);                 \
    PORTR_CLEAR(pin);               \
} while(0)

#define PIN_AS_OUTPUT_LOW(pin)  \
do {                            \
    PORTR_CLEAR(pin);           \
    DDR_SET(pin);               \
} while(0)

#define PIN_AS_OUTPUT_HIGH(pin) \
do {                            \
    PORTR_SET(pin);             \
    DDR_SET(pin);               \
} while(0)   

#endif /* PIN_MAP_H_ */
