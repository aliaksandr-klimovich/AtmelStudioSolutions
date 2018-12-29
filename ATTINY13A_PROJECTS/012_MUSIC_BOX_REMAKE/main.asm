;
; 011_MUSIC_BOX.asm
;
; Created: 24.12.2018 22:05:47
; Author : Aliaksandr
;

.def p_note_h = r12
.def p_note_l = r13
.def p_delay_h = r14
.def p_delay_l = r15

.def displacement = r16
.def tone = r17
.def note = r18
.def temp = r19
.def wd_reset = r20
.def wd_to = r21
.def ti_clk_sel = r22
.def ti_com_en = r23
.def delay = r24
.def zero = r25
;.def	XL	= r26
;.def	XH	= r27

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
        ; ---- ---- SETUP ---- ----

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
        ;ldi     temp, RAMEND
        ;out     SPL, temp

        ; Disable comparator
        ldi     temp, (1 << ACD)
        out     ACSR, temp

        ; Configure WDT
        ;cli
        ;wdr
        ;in      temp, WDTCR
        ;ori     temp, (1 << WDCE) | (1 << WDE)
        ;out     WDTCR, temp
        ;ldi     wd_to, (1 << WDTIE) | (1 << WDP2)
        ;out     WDTCR, wd_to
        ;sei

        ; Configure timer
m1:     out     TCCR0A, zero
        out     TCCR0B, zero
        ldi     ti_com_en, com_en

        ; Read KEY1
m2:     in      temp, PINB
        sbrc    temp, PINB1     ; check KEY1
        rjmp    m2

        ; Read tempo
        ldi     ZH, high(melody01 * 2)
        ldi     ZL, low(melody01 * 2)

        lpm     p_delay_l, Z+
        lpm     p_delay_h, Z+

m3:     lpm     note, Z+    ; read note

        cpi     note, 0xFF  ; end of melody
        breq    m1

        mov     p_note_h, ZH  ; store Z
        mov     p_note_l, ZL

        mov     displacement, note
        andi    displacement, 0b00011111    ; extract displacement (tone)

        mov     delay, note
        andi    delay, 0b11100000   ; extract delay
        swap    delay
        lsr     delay

        ldi     ZH, high(tone_tb * 2)
        ldi     ZL, low(tone_tb * 2)
        add     ZL, displacement
        adc     ZH, zero
        lpm     tone, Z

        cpi     displacement, clock8_start
        brlo    set_clk64
set_clk8:
        ldi     ti_clk_sel, clk8
        rjmp    m4
set_clk64:
        ldi     ti_clk_sel, clk64
m4:
        out     OCR0A, tone
        out     TCCR0A, ti_com_en
        out     TCCR0B, ti_clk_sel

        mov     ZH, p_delay_h
        mov     ZL, p_delay_l
        add     ZL, delay
        adc     ZH, zero
        lpm     delay, Z
        dec     delay
        rcall   make_delay

        ldi     delay, 1
        out     OCR0A, zero
        out     TCCR0A, zero
        rcall   make_delay

        ; restore Z
        mov     ZH, p_note_h
        mov     ZL, p_note_l

        rjmp    m3

; ---- ---- MAIN END, FUNCTION DEFINITIONS START ---- ----

make_delay:
        push    delay
        ldi     wd_to, 7    ; wdt timer = 2s (WDP2 + WDP1 + WDP0)
s1:
        lsl     delay
        brcc    s2

        rcall   set_wdt_prescaler
        rcall   wait_wdi

s2:
        dec     wd_to
        brge    s1

        pop     delay
        ret

wait_wdi:
        ldi     wd_reset, 0
s3:
        cpi     wd_reset, 1
        brne    s3
        ret

set_wdt_prescaler:
        push    wd_to
        ori     wd_to, (1 << WDTIE)
        cli
        wdr
        in      temp, WDTCR
        ori     temp, (1 << WDCE) | (1 << WDE)
        out     WDTCR, temp
        out     WDTCR, wd_to
        sei
        pop     wd_to
        ret

WATCHDOG:
        ldi     wd_reset, 1
        reti


; ---- ---- TONE TABLES ---- ----
;
.equ clock8_start = 0b10

;...         ... 00000, 00001, 00010, 00011, 00100, 00101, 00110, 00111, 01000, 01001, 01010, 01011, 01100, 01101, 01110, 01111, 10000, 10001, 10010, 10011, 10100, 10101, 10110, 10111, 11000, 11001, 11010, 11011, 11100, 11101, 11110, 11111
;...         ... C5,    C#5,   D5,    D#5,   E5,    F5,    F#5,   G5,    G#5,   A5,    A#5,   B5,    C6,    C#6,   D6,    D#6,   E6,    F6,    F#6,   G6,    G#6,   A6,    A#6,   B6,    C7,    C#7,   D7,    D#7,   E7,    F7,    F#7,   G7
tone_tb:     .db 36,    34,    255,   241,   228,   215,   203,   191,   181,   170,   161,   152,   143,   135,   128,   121,   114,   107,   101,   96,    90,    85,    80,    76,    72,    68,    64,    60,    57,    54,    51,    48
;clock_s_tb: .db clk64, clk64, clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8,  clk8


; ---- ---- TEMPO TABLES ---- ----
;
; 1      1/2    1/4    1/8    1/16   1/32   1/64   1/128
; 0b000  0b001  0b010  0b011  0b100  0b101  0b110  0b111

;               8s          4s          2s          1s          500ms       250ms       125ms       64ms
delay_tb0:  .db 0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001
;               12s         6s          3s          1.5s        750ms       375ms       190ms       -
delay_tb1:  .db 0b11000000, 0b01100000, 0b00110000, 0b00011000, 0b00001100, 0b00000110, 0b00000011, 0
;               10s         5s          2.5s        1.25s       625ms       312ms
delay_tb2:  .db 0b10100000, 0b01010000, 0b00101000, 0b00010100, 0b00001010, 0b00000101


; ---- ---- MELODIES ---- ----

;             >>                                <<
; ~*~-----~*~>>   We wish you a mery christmas   <<~~~*~-----~*~~~
;             >>                                <<
melody01:   ; (1) = 1.5s
            .dw (delay_tb1 * 2) + 3 ; tempo
            ;   1/4 D4      1/4 G4      1/8 G4      1/8 A4      1/8 G4      1/8 F#4     1/4 E4      1/4 C4
            .db 0b01000010, 0b01000111, 0b01100111, 0b01101001, 0b01100111, 0b01100110, 0b01000100, 0b01000000
            ;   1/4 E4      1/4 A4      1/8 A4      1/8 B4      1/8 A4      1/8 G4      1/4 F#4     1/4 D4
            .db 0b01000100, 0b01001001, 0b01101001, 0b01101011, 0b01101001, 0b01100111, 0b01000110, 0b01000010
            ;   1/4 F#4     1/4 B4      1/8 B4      1/8 C5      1/8 B4      1/8 A4      1/4 G4      1/4 E4
            .db 0b01000110, 0b01001011, 0b01101011, 0b01101100, 0b01101011, 0b01101001, 0b01000111, 0b01000100
            ;   1/8 D4      1/8 D4      1/4 E4      1/4 A4      1/4 F#4     1/2 G4      1/4 D4      1/4 G4
            .db 0b01100010, 0b01100010, 0b01000100, 0b01001001, 0b01000110, 0b00100111, 0b01000010, 0b01000111
            ;   1/4 G4      1/4 G4      1/2 F#4     1/4 F#4     1/4 G4      1/4 F#4     1/4 E4      1/2 D4
            .db 0b01000111, 0b01000111, 0b00100110, 0b01000110, 0b01000111, 0b01000110, 0b01000100, 0b00100010
            ;   1/4 A4      1/4 B4      1/8 A4      1/8 A4      1/8 G4      1/8 G4      1/4 D5      1/4 D4
            .db 0b01001001, 0b01001011, 0b01101001, 0b01101001, 0b01100111, 0b01100111, 0b01001110, 0b01000010
            ;   1/8 D4      1/8 D4      1/4 E4      1/4 A4      1/4 F#4     1/2 G4      end         -
            .db 0b01100010, 0b01100010, 0b01000100, 0b01001001, 0b01000110, 0b00100111, 0xFF,       0

;        /                            \
;  +----/    Tokyo ghoul - Unravel     \-----+
;       \                              /
melody02:   ;
            .dw (delay_tb0 * 2) + 2
            ;
            .db 0, 0

; todo:
;   1) delay enhacement, remake delay table
;   2) handle pauses
;   3)

;  0 0 0 0  0 0 0 0         delay
;  |              |
;  |              +--->  16ms
;  +--->  2s




