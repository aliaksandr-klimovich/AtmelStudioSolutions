/*
 * pin_map.h
 *
 * Created: 07.07.2019 10:46:07
 *  Author: Aliaksandr
 */

#ifndef PIN_MAP_H_
#define PIN_MAP_H_

typedef struct {
    volatile uint8_t *DDR_ADDR;
    volatile uint8_t *PORT_ADDR;
    volatile uint8_t *PIN_ADDR;
    uint8_t PIN_NUM;
} PIN;

#define DDR_SET_P(pin)    (*(pin->DDR_ADDR)  |= (1 << pin->PIN_NUM))
#define PORT_SET_P(pin)   (*(pin->PORT_ADDR) |= (1 << pin->PIN_NUM))
#define PIN_SET_P(pin)    (*(pin->PIN_ADDR)  |= (1 << pin->PIN_NUM))

#define DDR_CLEAR_P(pin)  (*(pin->DDR_ADDR)  &= ~(1 << pin->PIN_NUM))
#define PORT_CLEAR_P(pin) (*(pin->PORT_ADDR) &= ~(1 << pin->PIN_NUM))
#define PIN_CLEAR_P(pin)  (*(pin->PIN_ADDR)  &= ~(1 << pin->PIN_NUM))

#define PORT_CHECK_P(pin) (*pin->PORT_ADDR & (1 << pin->PIN_NUM))
#define PIN_CHECK_P(pin)  (*pin->PIN_ADDR  & (1 << pin->PIN_NUM))

#define PORT_READ_P(pin)  (PORT_CHECK_P(pin) ? 1 : 0)
#define PIN_READ_P(pin)   (PIN_CHECK_P(pin) ? 1 : 0)

#define DDR_SET(pin)    (*(pin.DDR_ADDR)  |= (1 << pin.PIN_NUM))
#define PORT_SET(pin)   (*(pin.PORT_ADDR) |= (1 << pin.PIN_NUM))
#define PIN_SET(pin)    (*(pin.PIN_ADDR)  |= (1 << pin.PIN_NUM))

#define DDR_CLEAR(pin)  (*(pin.DDR_ADDR)  &= ~(1 << pin.PIN_NUM))
#define PORT_CLEAR(pin) (*(pin.PORT_ADDR) &= ~(1 << pin.PIN_NUM))
#define PIN_CLEAR(pin)  (*(pin.PIN_ADDR)  &= ~(1 << pin.PIN_NUM))

#define PORT_CHECK(pin) (*(pin.PORT_ADDR) & (1 << pin.PIN_NUM))
#define PIN_CHECK(pin)  (*(pin.PIN_ADDR)  & (1 << pin.PIN_NUM))

#define PORT_READ(pin)  (PORT_CHECK(pin) ? 1 : 0)
#define PIN_READ(pin)   (PIN_CHECK(pin) ? 1 : 0)

#endif /* PIN_MAP_H_ */
