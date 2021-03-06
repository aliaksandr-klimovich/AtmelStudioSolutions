;
; 04_ASM_LED_SWITCH_WITH_BUTTON.asm
;
; Created: 16-Dec-18 09:09:09 PM
; Author : Aliaksandr
;



.def temp = R16

.cseg
.org 0

    ldi     temp, (1 << DDB0)   ; set LED1 as output
    out     DDRB, temp          ; and KEY1 as input
    ldi     temp, (1 << PORTB0) | (1 << PORTB1) ; disable LED1
    out     PORTB, temp                         ; and pull-up KEY1

main:
    in      temp, PINB      ; read PINB
    sbrc    temp, PINB1     ; if KEY1 is not pressed
    rjmp    main            ; jump to main

    sbi     PINB, PINB0     ; otherwise flip LED1
m2:
    in      temp, PINB      ; read PINB
    sbrs    temp, PINB1     ; if KEY1 is pressed
    rjmp    m2              ; jump to m2
    rjmp    main            ; otherwise jump to main
