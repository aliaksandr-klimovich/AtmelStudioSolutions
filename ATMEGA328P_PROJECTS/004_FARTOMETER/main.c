#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


/*
 * Helpers
 */

enum TIMER0_PRESCALER
{
    TIMER0_PRESCALER_0    = (0 << CS02) | (0 << CS01) | (0 << CS00),  // timer0 stopped
    TIMER0_PRESCALER_1    = (0 << CS02) | (0 << CS01) | (1 << CS00),  // clk / 1 (no prescaling)
    TIMER0_PRESCALER_8    = (0 << CS02) | (1 << CS01) | (0 << CS00),  // clk / 8
    TIMER0_PRESCALER_64   = (0 << CS02) | (1 << CS01) | (1 << CS00),  // clk / 64
    TIMER0_PRESCALER_256  = (1 << CS02) | (0 << CS01) | (0 << CS00),  // clk / 256
    TIMER0_PRESCALER_1024 = (1 << CS02) | (0 << CS01) | (1 << CS00),  // clk / 1024
};

#define CTC_MODE (1 << WGM01)

#define SET_BIT(reg, bit)   ((reg) |=  (1 << (bit)))
#define CLEAR_BIT(reg, bit) ((reg) &= ~(1 << (bit)))

#define PIN_AS_OUTPUT_LOW(ddr, portr, pin_id)   \
do                                              \
{                                               \
    CLEAR_BIT(portr, pin_id);                   \
    SET_BIT(ddr, pin_id);                       \
}                                               \
while(0)

#define PIN_AS_OUTPUT_HIGH(ddr, portr, pin_id)  \
do                                              \
{                                               \
    SET_BIT(portr, pin_id);                     \
    SET_BIT(ddr, pin_id);                       \
}                                               \
while(0)

#define PIN_AS_INPUT_NO_PULLUP(ddr, portr, pin_id)  \
do                                                  \
{                                                   \
    CLEAR_BIT(ddr, pin_id);                         \
    CLEAR_BIT(portr, pin_id);                       \
}                                                   \
while(0)

#define PIN_AS_INPUT_PULLUP(ddr, portr, pin_id) \
do                                              \
{                                               \
    CLEAR_BIT(ddr, pin_id);                     \
    SET_BIT(portr, pin_id);                     \
}                                               \
while(0)

#define READ_PIN(pinr,  pin_id) ((pinr) & (1 << (pin_id)))
#define READ_PORT(portr, pin_id) ((portr) & (1 << (pin_id)))


/*
 * CO2 sensor
 */

enum CO2_SENSOR_MEASUREMENT_STATUS
{
    CO2_SENSOR_MEASUREMENT_DISABLED,
    CO2_SENSOR_MEASUREMENT_TRIGGERED,
    CO2_SENSOR_MEASUREMENT_DETECTING_LOW_LEVEL,
    CO2_SENSOR_MEASUREMENT_DETECTING_HIGH_LEVEL,
    CO2_SENSOR_MEASUREMENT_COUNTING_TH_DETECTING_LOW_LEVEL,
    CO2_SENSOR_MEASUREMENT_COUNTING_TL_DETECTING_HIGH_LEVEL,
    CO2_SENSOR_MEASUREMENT_DONE,
    CO2_SENSOR_MEASUREMENT_ERROR,
};

enum CO2_SENSOR_MEASUREMENT_STATUS CO2_sensor_measurement_status;
uint16_t CO2_sensor_th;
uint16_t CO2_sensor_tl;
const uint16_t CO2_sensor_t_max_err = 5000;

#define CO2_SENSOR_DDR          DDRB
#define CO2_SENSOR_PORTR        PORTB
#define CO2_SENSOR_PINR         PINB
#define CO2_SENSOR_PIN_ID       0
#define INIT_CO2_SENSOR_PIN()   PIN_AS_INPUT_PULLUP(CO2_SENSOR_DDR, CO2_SENSOR_PORTR, CO2_SENSOR_PIN_ID)
#define READ_CO2_SENSOR_PIN()   READ_PIN(CO2_SENSOR_PINR, CO2_SENSOR_PIN_ID)


/*
 * LEDs
 */

#define GREEN_LED_DDR       DDRD
#define GREEN_LED_PORTR     PORTD
#define GREEN_LED_PINR      PIND
#define GREEN_LED_PIN_ID    3

#define YELLOW_LED_DDR      DDRD
#define YELLOW_LED_PORTR    PORTD
#define YELLOW_LED_PINR     PIND
#define YELLOW_LED_PIN_ID   4

#define RED_LED_DDR         DDRD
#define RED_LED_PORTR       PORTD
#define RED_LED_PINR        PIND
#define RED_LED_PIN_ID      5

#define BUILT_IN_LED_DDR    DDRB
#define BUILT_IN_LED_PORTR  PORTB
#define BUILT_IN_LED_PIN    PINB
#define BUILT_IN_LED_PIN_ID 5

#define INIT_LED_PINS()                                                             \
do                                                                                  \
{                                                                                   \
    PIN_AS_OUTPUT_LOW(GREEN_LED_DDR,    GREEN_LED_PORTR,    GREEN_LED_PIN_ID);      \
    PIN_AS_OUTPUT_LOW(YELLOW_LED_DDR,   YELLOW_LED_PORTR,   YELLOW_LED_PIN_ID);     \
    PIN_AS_OUTPUT_LOW(RED_LED_DDR,      RED_LED_PORTR,      RED_LED_PIN_ID);        \
    PIN_AS_OUTPUT_LOW(BUILT_IN_LED_DDR, BUILT_IN_LED_PORTR, BUILT_IN_LED_PIN_ID);   \
}                                                                                   \
while(0)

#define SET_GREEN_LED() SET_BIT(GREEN_LED_PORTR, GREEN_LED_PIN_ID)
#define SET_YELLOW_LED() SET_BIT(YELLOW_LED_PORTR, YELLOW_LED_PIN_ID)
#define SET_RED_LED() SET_BIT(RED_LED_PORTR, RED_LED_PIN_ID)
#define SET_BUILDT_IN_LED() SET_BIT(BUILT_IN_LED_PORTR, BUILT_IN_LED_PIN_ID)

#define CLEAR_GREEN_LED() CLEAR_BIT(GREEN_LED_PORTR, GREEN_LED_PIN_ID)
#define CLEAR_YELLOW_LED() CLEAR_BIT(YELLOW_LED_PORTR, YELLOW_LED_PIN_ID)
#define CLEAR_RED_LED() CLEAR_BIT(RED_LED_PORTR, RED_LED_PIN_ID)
#define CLEAR_BUILDT_IN_LED() CLEAR_BIT(BUILT_IN_LED_PORTR, BUILT_IN_LED_PIN_ID)

#define SET_ONLY_GREEN_LED()    \
do                              \
{                               \
    SET_GREEN_LED();            \
    CLEAR_YELLOW_LED();         \
    CLEAR_RED_LED();            \
}                               \
while(0)

#define SET_ONLY_YELLOW_LED()   \
do                              \
{                               \
    CLEAR_GREEN_LED();          \
    SET_YELLOW_LED();           \
    CLEAR_RED_LED();            \
}                               \
while(0)

#define SET_ONLY_RED_LED()  \
do                          \
{                           \
    CLEAR_GREEN_LED();      \
    CLEAR_YELLOW_LED();     \
    SET_RED_LED();          \
}                           \
while(0)

#define SET_ALL_LEDS()  \
do                      \
{                       \
    SET_GREEN_LED();    \
    SET_YELLOW_LED();   \
    SET_RED_LED();      \
}                       \
while(0)

#define CLEAR_ALL_LEDS()    \
do                          \
{                           \
    CLEAR_GREEN_LED();      \
    CLEAR_YELLOW_LED();     \
    CLEAR_RED_LED();        \
}                           \
while(0)


/*
 * Interrupts
 */

ISR(TIMER0_COMPA_vect)
{
    switch (CO2_sensor_measurement_status)
    {
        case CO2_SENSOR_MEASUREMENT_DISABLED:
        case CO2_SENSOR_MEASUREMENT_DONE:
        case CO2_SENSOR_MEASUREMENT_ERROR:
            break;

        case CO2_SENSOR_MEASUREMENT_TRIGGERED:
            CO2_sensor_th = 0;
            CO2_sensor_tl = 0;
            CO2_sensor_measurement_status = CO2_SENSOR_MEASUREMENT_DETECTING_LOW_LEVEL;
            // no break;

        case CO2_SENSOR_MEASUREMENT_DETECTING_LOW_LEVEL:
            if (READ_CO2_SENSOR_PIN() == 0)
            {
                CO2_sensor_tl = 0;  // CO2_sensor_tl is the error indicator
                CO2_sensor_measurement_status = CO2_SENSOR_MEASUREMENT_DETECTING_HIGH_LEVEL;
            }
            else
            {
                if(++CO2_sensor_tl == CO2_sensor_t_max_err)
                {
                    CO2_sensor_measurement_status = CO2_SENSOR_MEASUREMENT_ERROR;
                }
            }
            break;

        case CO2_SENSOR_MEASUREMENT_DETECTING_HIGH_LEVEL:
            if (READ_CO2_SENSOR_PIN() != 0)
            {
                CO2_sensor_th = 0;  // CO2_sensor_th is the error indicator
                CO2_sensor_measurement_status = CO2_SENSOR_MEASUREMENT_COUNTING_TH_DETECTING_LOW_LEVEL;
            }
            else
            {
                if(++CO2_sensor_th == CO2_sensor_t_max_err)
                {
                    CO2_sensor_measurement_status = CO2_SENSOR_MEASUREMENT_ERROR;
                }
            }
            break;

        case CO2_SENSOR_MEASUREMENT_COUNTING_TH_DETECTING_LOW_LEVEL:
            if (++CO2_sensor_th == CO2_sensor_t_max_err)
            {
                CO2_sensor_measurement_status = CO2_SENSOR_MEASUREMENT_ERROR;
            }
            else
            {
                if (READ_CO2_SENSOR_PIN() == 0)
                {
                    CO2_sensor_measurement_status = CO2_SENSOR_MEASUREMENT_COUNTING_TL_DETECTING_HIGH_LEVEL;
                }
            }
            break;

        case CO2_SENSOR_MEASUREMENT_COUNTING_TL_DETECTING_HIGH_LEVEL:
            if (++CO2_sensor_tl == CO2_sensor_t_max_err)
            {
                CO2_sensor_measurement_status = CO2_SENSOR_MEASUREMENT_ERROR;
            }
            else
            {
                if (READ_CO2_SENSOR_PIN() != 0)
                {
                    CO2_sensor_measurement_status = CO2_SENSOR_MEASUREMENT_DONE;
                }
            }
            break;
    }

    if (CO2_sensor_measurement_status == CO2_SENSOR_MEASUREMENT_DISABLED ||
        CO2_sensor_measurement_status == CO2_SENSOR_MEASUREMENT_DONE ||
        CO2_sensor_measurement_status == CO2_SENSOR_MEASUREMENT_ERROR)
    {
        TCCR0B = 0;  // Disable timer0
    }
}


int main(void)
{
    // Disable all interrupts
    cli();

    // Disable analog comparator
    ACSR = (1 << ACD);

    // Enable sleep possibility
    sleep_enable();

    // Configure pins
    INIT_CO2_SENSOR_PIN();
    INIT_LED_PINS();

    // Initialize timer0
    OCR0A = 249;  // to have interrupt each 1ms
    TIMSK0 = (1 << OCIE0A);  // enable timer0 interrupt

    // Enable interrupts
    sei();

    while(1)
    {
        // Set state machine to make measurement
        CO2_sensor_measurement_status = CO2_SENSOR_MEASUREMENT_TRIGGERED;

        // Reset timer
        TCNT0 = 0;

        // Enable timer
        TCCR0B = TIMER0_PRESCALER_64 | CTC_MODE;

        // Wait till measurement is completed
        do
        {
            sleep_cpu();
        }
        while (CO2_sensor_measurement_status != CO2_SENSOR_MEASUREMENT_DONE &&
               CO2_sensor_measurement_status != CO2_SENSOR_MEASUREMENT_ERROR);

        // Here timer0 is disabled, no interrupt can occur

        // Check for errors
        if (CO2_sensor_measurement_status == CO2_SENSOR_MEASUREMENT_ERROR)
        {
            SET_BUILDT_IN_LED();
            CLEAR_ALL_LEDS();
            continue;  // retry
        }
        else
        {
            CLEAR_BUILDT_IN_LED();
        }

        // Here CO2_sensor_measurement_status can be only CO2_SENSOR_MEASUREMENT_DONE

        // Calculate CO2 ppm (particles per million)
        uint32_t Cppm = (2000 * ((uint32_t)CO2_sensor_th - 2)) / ((uint32_t)CO2_sensor_th + (uint32_t)CO2_sensor_tl - 4);

        // Set LEDs accordingly
        #define RANGE1 500
        #define RANGE2 800
        #define RANGE3 1100
        #define RANGE4 1400

        if (0 <= Cppm && Cppm < RANGE1)
        {
            SET_ONLY_GREEN_LED();
        }
        else if (RANGE1 <= Cppm && Cppm < RANGE2)
        {
            SET_GREEN_LED();
            SET_YELLOW_LED();
            CLEAR_RED_LED();
        }
        else if (RANGE2 <= Cppm && Cppm < RANGE3)
        {
            SET_ONLY_YELLOW_LED();
        }
        else if (RANGE3 <= Cppm && Cppm < RANGE4)
        {
            CLEAR_GREEN_LED();
            SET_YELLOW_LED();
            SET_RED_LED();
        }
        else  // CO2ppm >= RANGE4
        {
            SET_ONLY_RED_LED();
        }
    }
}
