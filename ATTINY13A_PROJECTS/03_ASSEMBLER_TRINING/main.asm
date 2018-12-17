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

    ldi     temp, (1 << DDB0)
    out     DDRB, temp
    ldi     temp, (1 << PORTB1)
    out     PORTB, temp

main:
    in      temp, PINB
    sbrs    temp, PINB1
    cbi     PORTB, PORTB0
    sbi     PORTB, PORTB0
    rjmp    main
