;
; 05_LED_SWITCH_BUTTON_BOUNCE.asm
;
; Created: 16-Dec-18 10:47:31 PM
; Author : Aliaksandr
;


.def temp = R16     ; temporary variable
.def loop1 = R17    ; debounce counter
.def loop2 = R18    ; debounce counter

.cseg
.org 0

    ldi     temp, (1 << DDB0)   ; LED1 as output, KEY1 as input
    out     DDRB, temp
    ldi     temp, (1 << PORTB0) | (1 << PORTB1) ; disable LED1 , pull-up KEY1
    out     PORTB, temp
    rcall   wait

main:
    in      temp, PINB      ; read KEY1
    sbrc    temp, PINB1     ; if KEY1 is not pressed
    rjmp    main            ; jump back
    rcall   wait            ; otherwise (KEY1 is pressed) wait

    sbi     PINB, PINB0     ; flip LED1
m2:
    in      temp, PINB      ; read KEY1
    sbrs    temp, PINB1     ; if KEY1 is pressed
    rjmp    m2              ; jump back
    rcall   wait            ; otherwise (KEY1 is not pressed) wait
    rjmp    main            ; cycle main

wait:                       ; 0.0541875 [s] ?
    ldi     loop1, 255
    ldi     loop2, 255
wt1:
    dec     loop1
    brne    wt1
    ldi     loop1, 255
    dec     loop2
    brne    wt1
    ret