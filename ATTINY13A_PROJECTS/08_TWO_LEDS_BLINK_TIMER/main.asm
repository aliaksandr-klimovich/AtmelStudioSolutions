;
; 08_TWO_LEDS_BLINK_TIMER.asm
;
; Created: 20.12.2018 12:59:18
; Author : Aliaksandr
;

.def temp = R16

.def loop0 = R17
.def loop1 = R18

.equ ti0 = 1
.equ ti1 = 1
.equ tic = 100      ; 9'600'000 / 8 / 1024 / 100

.def led1 = R20
.def led2 = R21
.def noLed = R22
.def allLeds = R23

        ; Configure LED1 and LED2 as outputs and disable them
        ; Also connect pull-up resistor to KEY1

        ldi     temp, (1 << DDB0) | (1 << DDB4)
        out     DDRB, temp
        ldi     temp, (1 << PORTB0) | (1 << PORTB4) | (1 << PORTB1)
        mov     noled, temp
        out     DDRB, temp

        ; Configure timer

        ldi     temp, (1 << CS00) | (1 << CS02)     ; clk / 1024
        out     TCCR0B, temp

        ; Disable comparator

        ldi     temp, (1 << ACD)
        out     ACSR, temp

        ; Assign LED pins to registers, KEY1 should stay pulled up

        ldi     led2, (1 << PORTB0) | (1 << PORTB1)                     ; LED2 enabled
        ldi     led1, (1 << PORTB4) | (1 << PORTB1)                     ; LED1 enabled
        ldi     noLed, (1 << PORTB0) | (1 << PORTB4)| (1 << PORTB1)     ; LEDs disabled
        ldi     allLeds, (1 << PORTB1)                                  ; LEDs enabled

        ; Read KEY1

main:   in      temp, PINB
        sbrs    temp, PINB1
        rjmp    mode_key_down

        ; mode_key_up is:
        ;   - Switch all LEDs on
        ;   - Disable LED2 (LED1 stays on)
        ;   - Disable LED1 (all LEDs off)
        ;   - Wait double time of delay between switches
        ;
        ; mode_key_down is same as mode_key_up,
        ; but disable LED1 first instead of LED2 and set LED2 afterwards

mode_key_up:
        out     PORTB, allLeds
        rcall   wait
        out     PORTB, led1
        rcall   wait
        out     PORTB, noLed
        rcall   wait
        rcall   wait
        rjmp    main

mode_key_down:
        out     PORTB, allLeds
        rcall   wait
        out     PORTB, led2
        rcall   wait
        out     PORTB, noLed
        rcall   wait
        rcall   wait
        rjmp    main

        ; Use  timer to wait
        ; Note: this is a overcomplicated loop, so consider to use the call only from w_ti

wait:   ldi     loop0, ti0
        ldi     loop1, ti1
w_loop0:
        tst     loop0
        breq    w_loop1

w_ti:   ldi     temp, 0
        out     TCNT0, temp
w_ti_r: in      temp, TCNT0
        cpi     temp, tic
        brlo    w_ti_r

        dec     loop0
        rjmp    w_loop0

w_loop1:
        ldi     loop0, ti0
        dec     loop1
        brne    w_ti
        ret