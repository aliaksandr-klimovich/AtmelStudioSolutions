;
; 09_TWO_LEDS_BLINK_INTERRUPT.asm
;
; Created: 21.12.2018 16:48:50
; Author : Aliaksandr
;

; Registers aliases

.def temp = r16
.def rab = r17
.def cnt = r18

; Data segment. Reserve one byte.

.dseg
.org 0x60

        all_leds:   .byte       1
        led1:       .byte       1
        led2:       .byte       1
        no_led:     .byte       1

; Code segment

.cseg
.org 0

        ; Interrupt vectors

        rjmp    RESET
        reti    ;rjmp    EXT_INT0
        reti    ;rjmp    PCINT0
        reti    ;rjmp    TIM0_OVF
        reti    ;rjmp    EE_RDY
        reti    ;rjmp    ANA_COMP
        rjmp    TIM0_COMPA
        reti    ;rjmp    TIM0_COMPB
        reti    ;rjmp    WATCHDOG
        reti    ;rjmp    ADC_H

        ; Actual start of the program

RESET:

        ; Set stack pointer to the end of RAM

        ldi     temp, RAMEND
        out     SPL, temp

        ; Configure LED1, LED2, KEY1.
        ; LED1 as output, disabled.
        ; LED2 as output, disabled.
        ; KEY1 as intput, pull-up.

        ldi     temp, (1 << DDB0) | (1 << DDB4)
        out     DDRB, temp
        ldi     temp, (1 << PORTB0) | (1 << PORTB4) | (1 << PORTB1)
        out     PORTB, temp

        ; Configure timer

        ldi     temp, (1 << WGM01)                  ; CTC mode
        out     TCCR0A, temp
        ldi     temp, (1 << CS02) | (1 << CS00)     ; clk / 1024
        out     TCCR0B, temp
        ldi     temp, 75                            ; 
        out     OCR0A, temp
        ldi     temp, (1 << OCIE0A)
        out     TIMSK0, temp

        ; Disable comparator

        ldi     temp, (1 << ACD)
        out     ACSR, temp

        ; Init RAM

        ldi     rab, (1 << PORTB0) | (1 << PORTB4) | (1 << PORTB1)
        sts     no_led, rab
        ldi     rab, (1 << PORTB4) | (1 << PORTB1)
        sts     led1, rab
        ldi     rab, (1 << PORTB0) | (1 << PORTB1)
        sts     led2, rab
        ldi     rab, (1 << PORTB1)
        sts     all_leds, rab

        ldi     cnt, 0

        ; Enable interrupts

        sei

        ; Infinite loop

main:   rjmp    main

        ; Timer compare match interrupt

TIM0_COMPA:
        in      temp, PINB
        sbrs    temp, PINB1
        rjmp    mode2

mode1:  cpi     cnt, 0
        breq    en_led2
        cpi     cnt, 1
        breq    en_led1
        rjmp    dis_all_leds

mode2:  cpi     cnt, 0
        breq    en_led1
        cpi     cnt, 1
        breq    en_led2
        rjmp    dis_all_leds

en_all_leds:
        lds     rab, all_leds
        rjmp    w1
en_led1:
        lds     rab, led1
        rjmp    w1
en_led2:
        lds     rab, led2
        rjmp    w1
dis_all_leds:
        lds     rab, no_led

w1:     out     PORTB, rab
        inc     cnt
        cpi     cnt, 5
        brlo    w2
        ldi     cnt, 0
w2:
        reti