;
; 011_MUSIC_BOX.asm
;
; Created: 24.12.2018 22:05:47
; Author : Aliaksandr
;

.def temp3 = r14
.def temp2 = r15
.def temp = r16
.def cnt = r17
.def wd_reset = r18
.def wd_to = r19

.equ no_clk = 0
.equ clk1 = (1 << CS00)
.equ clk8 = (1 << CS01)
.equ clk64 = (1 << CS01) | (1 << CS00)
.equ clk256 = (1 << CS02)
.equ clk1024 = (1 << CS02) | (1 << CS00)

.equ com_en = (1 << COM0A0) | (1 << WGM01)  ; compare output mode A and CTC

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
        rjmp    WATCHDOG
        reti    ;rjmp    ADC_H

RESET:

        ; Configure buzzer (connected to LED1), LED2, KEY1, KEY2.
        ; Buzzer as output, disabled.
        ; LED2 as output, disabled.
        ; KEY1 as input, pull-up.
        ; KEY2 as input, pull-up.

        ldi     temp, (1 << DDB4) | (1 << DDB0)
        out     DDRB, temp
        ldi     temp, (1 << PORTB4) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0)
        out     PORTB, temp

        ; Set stack pointer to the end of RAM

        ldi     temp, RAMEND
        out     SPL, temp

        ; Disable comparator

        ldi     temp, (1 << ACD)
        out     ACSR, temp

        ; Configure WDT

        ;clr     wd_reset
        ;cli
        wdr
        in      temp, WDTCR
        ori     temp, (1 << WDCE) | (1 << WDE)
        out     WDTCR, temp
        ldi     wd_to, (1 << WDTIE) | (1 << WDP2)
        out     WDTCR, wd_to
        sei

        ; Configure timer

m1:     ldi     temp, no_clk
        out     TCCR0B, temp

        ldi     temp, 0
        out     TCCR0A, temp

        ; Read KEY1

m2:     in      temp, PINB
        sbrc    temp, PINB1     ; check KEY1
        rjmp    m2

        ; Read tone
        ldi     ZH, high(mel01 * 2)
        ldi     ZL, low(mel01 * 2)

m3:     lpm     temp, Z+

        cpi     temp, 0xff
        breq    m1

        andi    temp, 0b00011111

        push    ZH
        push    ZL
        ldi     YL, 0

        ldi     ZH, high(tone_tb * 2)
        ldi     ZL, low(tone_tb * 2)
        add     ZL, temp
        adc     ZH, YL
        lpm     temp2, Z

        ldi     ZH, high(clock_s_tb * 2)
        ldi     ZL, low(clock_s_tb * 2)
        add     ZL, temp
        adc     ZH, YL
        lpm     temp3, Z

        pop     ZL
        pop     ZH

        out     OCR0A, temp2

        ldi     temp, com_en
        out     TCCR0A, temp

        ;ldi     temp, clk8
        out     TCCR0B, temp3

        wdr
        out     WDTCR, wd_to
        ldi     wd_reset, 0
m4:     cpi     wd_reset, 1
        brne    m4
        rjmp    m3

WATCHDOG:
        ldi     wd_reset, 1
        reti

addw:   ret

;...        ... 00000, 00001, 00010, 00011, 00100, 00101, 00110, 00111, 01000, 01001, 01010, 01011, 01100, 01101, 01110, 01111, 10000, 10001, 10010, 10011, 10100, 10101, 10110, 10111, 11000, 11001, 11010, 11011, 11100, 11101, 11110, 11111
;...        ... C5,    C?5,   D5,    D?5,   E5,    F5,    F?5,   G5,    G?5,   A5,    A?5,   B5,    C6,    C?6,   D6,    D?6,   E6,    F6,    F?6,   G6,    G?6,   A6,    A?6,   B6,    C7,    C?7,   D7,    D?7,   E7,    F7,    F?7,   G7
tone_tb:    .db 36,    34,    255,   241,   228,   215,   203,   191,   181,   170,   161,   152,   143,   135,   128,   121,   114,   107,   101,   96,    90,    85,    80,    76,    72,    68,    64,    60,    57,    54,    51,    48
clock_s_tb: .db clk64, clk64, clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8

; 0b000 - 1
; 0b001 - 1/2
; 0b010 - 1/4
; 0b011 - 1/8
; 0b100 - 1/16
; 0b101 - 1/32
; 0b110 - 1/64
; 0b111 - 1/128

; 1/4 = 60      8s                         4s
delay_tb:   .db (1 << WDP3) | (1 << WDP0), (1 << WDP3)
            ;   2s                                       1s
            .db (1 << WDP2) | (1 << WDP1) | (1 << WDP0), (1 << WDP2) | (1 << WDP1)
            ;   0.5s                       0.25s
            .db (1 << WDP2) | (1 << WDP0), (1 << WDP2)
            ;   0.125s                     64ms         32ms         16ms
            .db (1 << WDP1) | (1 << WDP0), (1 << WDP1), (1 << WDP0), 0
            

mel01:      ; We wish you a mery christmas
            ;
            ;   1/4 D5      1/4 G5      1/8 G4      1/8 A4      1/8 G4      1/8 F#4     1/4 E4      1/4 C4
            .db 0b01000010, 0b01000111, 0b01100111, 0b01101001, 0b01100111, 0b01100110, 0b01000100, 0b01000000
            ;   1/4 E4      1/4 A4      1/8 A4      1/8 B4      1/8 A4      1/8 G4      1/4 F#4     1/4 D5
            .db 0b01000100, 0b01001001, 0b01101001, 0b01101011, 0b01101001, 0b01100111, 0b01000110, 0b01000010
            .db 0xff, 0xff