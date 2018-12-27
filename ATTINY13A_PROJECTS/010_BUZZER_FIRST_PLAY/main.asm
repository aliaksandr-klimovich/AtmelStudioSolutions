;
; 010_BUZZER_FIRST_PLAY.asm
;
; Created: 24.12.2018 0:00:33
; Author : Aliaksandr
;


; Registers aliases

.def temp = r16
.def cnt = r17

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
        reti    ;rjmp    TIM0_COMPA
        reti    ;rjmp    TIM0_COMPB
        reti    ;rjmp    WATCHDOG
        reti    ;rjmp    ADC_H

        ; Actual start of the program

RESET:

        ; Set stack pointer to the end of RAM

        ;ldi     temp, RAMEND
        ;out     SPL, temp

        ; Configure buzzer (connected to LED1), LED2, KEY1.
        ; Buzzer as output, disabled.
        ; LED2 as output, disabled.
        ; KEY1 as input, pull-up.

        ldi     temp, (1 << DDB4) | (1 << DDB0)
        out     DDRB, temp
        ldi     temp, (1 << PORTB4) | (1 << PORTB1) | (1 << PORTB0)
        out     PORTB, temp

        ; Disable comparator

        ldi     temp, (1 << ACD)
        out     ACSR, temp

        ; Configure timer

        ldi     temp,  (1 << CS01)      ; clkI/O/8 (From prescaler)
        out     TCCR0B, temp

        ; Initialize table offset

        ldi     cnt, 3  ; min = 0, max = 5

        ; Main cycle.
        ; Disable buzzer.

m1:     ldi     temp, 0
        out     TCCR0A, temp

        ; Read KEY1

m2:     in      temp, PINB
        sbrc    temp, PINB1
        rjmp    m1

        ; Load table address, add offset counter
        ; and place the result into the compare match register.

        ldi     ZL, low(tab * 2)
        mov     YL, cnt
        add     ZL, YL
        lpm     YL, Z
        out     OCR0A, YL

        ; Configure timer

        ldi     temp, (1 << COM0A0) | (1 << WGM01)  ; Toggle OC0A on Compare Match, CTC mode
        out     TCCR0A, temp

        rjmp    m2

;               0     1     2     3     4     5
tab:    .db     0xA0, 0x14, 0x32, 0x64, 0xC8, 0xFF       