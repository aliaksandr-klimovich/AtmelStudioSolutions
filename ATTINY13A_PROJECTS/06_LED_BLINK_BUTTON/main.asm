;
; 06_LED_BLINK_BUTTON.asm
;
; Created: 17-Dec-18 10:29:16 PM
; Author : Aliaksandr
;

.def temp = R16
.def loop1 = R17
.def loop2 = R18
.def loop3 = R19

.cseg
.org 0

    ldi     temp, (1 << DDB0)   ; LED1 as output
    out     DDRB, temp          ; KEY1 as input
    ldi     temp, (1 << PORTB0) | (1 << PORTB1) ; disable LED1
    out     PORTB, temp                         ; pull-up KEY1

start:
    sbi     PORTB, PORTB0
    in      temp, PINB
    sbrc    temp, PINB1
    rjmp    start

    cbi     PORTB, PORTB0
    rcall   wait1
    sbi     PORTB, PORTB0
    rcall   wait1
    rjmp    start

wait1:
    push    loop1
    push    loop2
    push    loop3

    ldi     loop3, 5
wt1:
    dec     loop3
    breq    wt4
    ldi     loop2, 50
wt2:
    dec     loop2
    breq    wt1
    ldi     loop1, 200
wt3:
    dec     loop1
    brne    wt3
    rjmp    wt2
wt4:
    pop     loop3
    pop     loop2
    pop     loop1
    ret      