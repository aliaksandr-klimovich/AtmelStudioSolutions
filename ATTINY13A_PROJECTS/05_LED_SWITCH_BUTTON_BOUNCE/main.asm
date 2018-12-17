;
; 05_LED_SWITCH_BUTTON_BOUNCE.asm
;
; Created: 16-Dec-18 10:47:31 PM
; Author : Aliaksandr
;


.def temp = R16     ; temporary variable
.def loop1 = R17    ; debounce timers
.def loop2 = R18

.cseg
.org 0
    ;ldi     temp, RAMEND
    ;out     SPL, temp

    ldi     temp, (1 << DDB0)   ; set LED1 as output, KEY1 as input
    out     DDRB, temp
    ldi     temp, (1 << PORTB0) | (1 << PORTB1) ; set LED1 disabled, KEY1 as input pull-up
    out     PORTB, temp
    rcall   wait

main:
    in      temp, PINB      ; read PINB
    sbrc    temp, PINB1     ; if KEY1 is not pressed
    rjmp    main            ; jump to main
    rcall   wait

    sbi     PINB, PINB0     ; otherwise flip LED1
m2:
    in      temp, PINB      ; read PINB
    sbrs    temp, PINB1     ; if KEY1 is pressed
    rjmp    m2              ; jump to m2
    rcall   wait
    rjmp    main            ; otherwise jump to main

wait:
    ;push    loop1
    ;push    loop2

    ldi     loop1, 255
    ldi     loop2, 255
wt1:
    dec     loop1
    brne    wt1
    ldi     loop1, 255
    dec     loop2
    brne    wt1

    ;pop     loop2
    ;pop     loop1
    ret