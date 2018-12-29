;
; 012_MUSIC_BOX_REMAKE.asm
;
; Created: 29.12.2018 12:11:10
; Author : Aliaksandr
;

.def tone_displacement = r10
.def delay_displacement = r11

.def ZH_copy = r12
.def ZL_copy = r13
.def delay_copy = r14
.def wd_to_copy = r15
; ---- ---- ---- ----
.def idx_tone_clk = r16
.def tone = r17
;.def ? = r18
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
;.equ clk1 = (1 << CS00)
.equ clk8 = (1 << CS01)
.equ clk64 = (1 << CS01) | (1 << CS00)
;.equ clk256 = (1 << CS02)
;.equ clk1024 = (1 << CS02) | (1 << CS00)

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

        ; Configure buzzer (connected to PB0), LED2, KEY1, KEY2.
        ; Buzzer as output, disabled.
        ; LED2 as output, disabled.
        ; KEY1 as input, pull-up.
        ; KEY2 as input, pull-up.
        ldi     temp, (1 << DDB4) | (1 << DDB0)
        out     DDRB, temp
        ldi     temp, (1 << PORTB4) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0)
        out     PORTB, temp

        ; Disable comparator
        ldi     temp, (1 << ACD)
        out     ACSR, temp

        ; Set stack pointer to the end of RAM
        ldi     temp, RAMEND
        out     SPL, temp

        ldi     zero, 0
        ldi     ti_com_en, com_en

        ; Configure timer
m1:     out     TCCR0A, zero
        out     TCCR0B, zero

m2:     in      temp, PINB      ; read and check KEY1
        sbrc    temp, PINB1     ;
        rjmp    m2

        ; Read melody config
        ldi     ZL, low(melody02 * 2)
        ldi     ZH, high(melody02 * 2)
        lpm     delay_displacement, Z+
        lpm     tone_displacement, Z+

m3:     lpm     delay, Z+           ; read delay code

;        cp      delay, delay_displacement
;        brsh    sub1
;        ldi     delay, 0
;        rjmp    sub2
;sub1:   sub     delay, delay_displacement
;sub2:

        lpm     idx_tone_clk, Z+    ; read index of tone / clk

        mov     ZH_copy, ZH  ; store Z
        mov     ZL_copy, ZL

        cpi     idx_tone_clk, 0xFF  ; end of melody
        breq    m1

        tst     idx_tone_clk        ; if pause
        brne    m4
        out     TCCR0A, zero
        out     TCCR0B, zero
        rjmp    m5

m4:     ldi     ZL, low(tone_tb * 2)
        ldi     ZH, high(tone_tb * 2)
        add     ZL, idx_tone_clk
        adc     ZH, zero
        lpm     tone, Z

        ldi     ZL, low(clock_tb * 2)
        ldi     ZH, high(clock_tb * 2)
        add     ZL, idx_tone_clk
        adc     ZH, zero
        lpm     ti_clk_sel, Z

        out     OCR0A, tone
        out     TCCR0A, ti_com_en
        out     TCCR0B, ti_clk_sel

m5:     rcall   make_delay

        ; restore Z
        mov     ZL, ZL_copy
        mov     ZH, ZH_copy

        rjmp    m3

; ---- ---- MAIN END, FUNCTION DEFINITIONS START ---- ----

make_delay:
        mov     delay_copy, delay
        ldi     wd_to, 7    ; wdt timer = 2s (WDP2 + WDP1 + WDP0)

s1:     lsl     delay
        brcc    s2

        rcall   set_wdt_prescaler
        rcall   wait_wdi

s2:     dec     wd_to
        brge    s1

        mov     delay, delay_copy
        ret

; ----

wait_wdi:
        ldi     wd_reset, 0
s3:
        cpi     wd_reset, 1
        brne    s3
        ret

; ----

set_wdt_prescaler:
        mov     wd_to_copy, wd_to
        ori     wd_to, (1 << WDTIE)
        cli
        wdr
        in      temp, WDTCR
        ori     temp, (1 << WDCE) | (1 << WDE)
        out     WDTCR, temp
        out     WDTCR, wd_to
        sei
        mov     wd_to, wd_to_copy
        ret

; ----

WATCHDOG:
        ldi     wd_reset, 1
        reti


; ---- ---- TONE TABLES ---- ----
;
;...      ... 00,    01,    02,    03,    04,    05,    06,    07,    08,    09,    0A,    0B,    0C,    0D,    0E,    0F,    10,    11,    12,    13,   14,   15,   16,   17,   18,   19,   1A,   1B,   1C,   1D,   1E,   1F,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   2A,   2B,   2C,   2D,   2E,   2F,   30,   31,   32,   33,   34,   35
;...      ... G3,    G#3,   A3,    A#3,   B3,    C4,    C#4,   D4,    D#4,   E4,    F4,    F#4,   G4,    G#4,   A4,    A#4,   B4,    C5,    C#5,   D5,   D#5,  E5,   F5,   F#5,  G5,   G#5,  A5,   A#5,  B5,   C6,   C#6,  D6,   D#6,  E6,   F6,   F#6,  G6,   G#6,  A6,   A#6,  B6,   C7,   C#7,  D7,   D#7,  E7,   F7,   F#7,  G7,   G#7,  A7,   A#7,  B7,   C8
tone_tb:  .db 96,    90,    85,    80,    76,    72,    68,    64,    60,    57,    54,    51,    48,    45,    43,    40,    38,    36,    34,    255,  241,  228,  215,  203,  191,  181,  170,  161,  152,  143,  135,  128,  121,  114,  107,  101,  96,   90,   85,   80,   76,   72,   68,   64,   60,   57,   54,   51,   48,   45,   43,   40,   38,   36
clock_tb: .db clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk64, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8, clk8


; ---- ---- MELODIES ---- ----
;
;        /                            \
;  +----/    Tokyo ghoul - Unravel     \-----+
;       \                              /
melody02:   ;   delay displacement, tone displacement
            .db 0, 0
            ;
            .db 0b0001_0000, 0x1B   ; 1/8 Bb5
            .db 0b0010_0000, 0x1D   ; 1/4 C6
            .db 0b0010_0000, 0x1B   ; 1/4 Bb5
            .db 0b0001_0000, 0x1A   ; 1/8 A5
            .db 0b0001_1111, 0x18   ; 1/8.... G5
            .db 0b0000_0001, 0      ; 1/128 -
            ; ----
            .db 0b0010_0000, 0x1D   ; 1/4 C6
            .db 0b0010_0000, 0x1B   ; 1/4 Bb5
            .db 0b0010_0000, 0x1A   ; 1/4 A5
            .db 0b0001_1000, 0x18   ; 1/8. G5
            .db 0b0000_1000, 0      ; 1/16 -
            ; ----
            .db 0b0001_0000, 0x18   ; 1/8 G5
            .db 0b0001_1000, 0x16   ; 1/8. F5
            .db 0b0000_1000, 0      ; 1/16 -
            .db 0b0001_0000, 0      ; 1/8 -
            .db 0b0000_1111, 0x14   ; 1/16... Eb5
            .db 0b0000_0001, 0      ; 1/128 -
            .db 0b0010_0000, 0x14   ; 1/4 Eb5
            .db 0b0001_0000, 0x16   ; 1/8 F5
            ; ----
            .db 0b0010_0000, 0x13   ; 1/4 D5
            .db 0b0110_0000, 0      ; 1/2. -
            ; ----
            .db 0, 0xFF
