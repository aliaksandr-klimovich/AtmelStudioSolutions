;
; 03_ASSEMBLER_TRINING.asm
;
; Created: 12/16/2018 1:11:14 PM
; Author : Aliaksandr
;

.include "tn13adef.inc"
.list

.def temp = R16

.cseg
.org 0

    ldi     temp, (1 << DDB0)       ; LED1
    out     DDRB, temp              ; as output
    ldi     temp, (1 << PORTB0) | (1 << PORTB1) ; disable LED1
    out     PORTB, temp                         ; and pull-up KEY1

main:
    in      temp, PINB      ; read KEY1
    sbrs    temp, PINB1     ; if KEY1 is pressed
    rjmp    led_on          ; enable LED1
    rjmp    led_off         ; otherwise disable LED1
led_on:
    cbi     PORTB, PORTB0
    rjmp    main
led_off:
    sbi     PORTB, PORTB0
    rjmp    main
