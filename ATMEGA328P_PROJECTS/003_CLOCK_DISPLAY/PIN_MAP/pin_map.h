/*
 * pin_map.h
 *
 * Created: 07.07.2019 10:46:07
 *  Author: Aliaksandr
 */


#ifndef PIN_MAP_H_
#define PIN_MAP_H_


typedef struct PIN_t {
    volatile uint8_t *DDR_ADDR;
    volatile uint8_t *PORT_ADDR;
    volatile uint8_t *PIN_ADDR;
    uint8_t PIN_NUM;
} PIN;


#define SET_DDR_P(pin)    (*(pin->DDR_ADDR)  |= (1 << pin->PIN_NUM))
#define SET_PORT_P(pin)   (*(pin->PORT_ADDR) |= (1 << pin->PIN_NUM))
#define SET_PIN_P(pin)    (*(pin->PIN_ADDR)  |= (1 << pin->PIN_NUM))

#define CLEAR_DDR_P(pin)  (*(pin->DDR_ADDR)  &= ~(1 << pin->PIN_NUM))
#define CLEAR_PORT_P(pin) (*(pin->PORT_ADDR) &= ~(1 << pin->PIN_NUM))
#define CLEAR_PIN_P(pin)  (*(pin->PIN_ADDR)  &= ~(1 << pin->PIN_NUM))

#define CHECK_PORT_P(pin) (*pin->PORT_ADDR & (1 << pin->PIN_NUM))
#define CHECK_PIN_P(pin)  (*pin->PIN_ADDR  & (1 << pin->PIN_NUM))

#define READ_PORT_P(pin)  (CHECK_PORT_P(pin) ? 1 : 0)
#define READ_PIN_P(pin)   (CHECK_PIN_P(pin) ? 1 : 0)


#define SET_DDR(pin)    (*(pin.DDR_ADDR)  |= (1 << pin.PIN_NUM))
#define SET_PORT(pin)   (*(pin.PORT_ADDR) |= (1 << pin.PIN_NUM))
#define SET_PIN(pin)    (*(pin.PIN_ADDR)  |= (1 << pin.PIN_NUM))

#define CLEAR_DDR(pin)  (*(pin.DDR_ADDR)  &= ~(1 << pin.PIN_NUM))
#define CLEAR_PORT(pin) (*(pin.PORT_ADDR) &= ~(1 << pin.PIN_NUM))
#define CLEAR_PIN(pin)  (*(pin.PIN_ADDR)  &= ~(1 << pin.PIN_NUM))

#define CHECK_PORT(pin) (*(pin.PORT_ADDR) & (1 << pin.PIN_NUM))
#define CHECK_PIN(pin)  (*(pin.PIN_ADDR)  & (1 << pin.PIN_NUM))

#define READ_PORT(pin)  (CHECK_PORT(pin) ? 1 : 0)
#define READ_PIN(pin)   (CHECK_PIN(pin) ? 1 : 0)


#endif /* PIN_MAP_H_ */
