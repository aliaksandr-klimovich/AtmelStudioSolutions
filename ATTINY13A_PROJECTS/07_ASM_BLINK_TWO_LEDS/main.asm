;
; 07_ASM_BLINK_TWO_LEDS.asm
;
; Created: 19.12.2018 22:08:37
; Author : Aliaksandr
;


.def temp = R16
.def loop1 = R17
.def loop2 = R18
.def loop3 = R19
.def led1 = R20
.def led2 = R21
.def noLed = R22
.def allLeds = R23

        ; Configure LED1 and LED2 as outputs and disable them
        ; Also connect pull-up resistor to KEY1

        ldi     temp, (1 << DDB0) | (1 << DDB4)
        out     DDRB, temp
        ldi     temp, (1 << PORTB0) | (1 << PORTB4) | (1 << PORTB1)
        mov     noled, temp
        out     PORTB, temp

        ; Disable comparator

        ldi     temp, (1 << ACD)
        out     ACSR, temp

        ; Assign LED pins to registers, KEY1 should stay pulled up

        ldi     led2, (1 << PORTB0) | (1 << PORTB1)                     ; LED2 enabled
        ldi     led1, (1 << PORTB4) | (1 << PORTB1)                     ; LED1 enabled
        ldi     noLed, (1 << PORTB0) | (1 << PORTB4)| (1 << PORTB1)     ; LEDs disabled
        ldi     allLeds, (1 << PORTB1)                                  ; LEDs enabled

        ; Read KEY1

main:   in      temp, PINB
        sbrs    temp, PINB1
        rjmp    mode2           ; mode 2 if KEY1 was pressed

        ; Mode 1 is:
        ;   - Switch all LEDs on
        ;   - Disable LED2 (LED1 stays on)
        ;   - Disable LED1 (all LEDs off)
        ;   - Wait triple time of delay between switches
        ;
        ; Mode 2 is same as mode 1, but disable LED1 first and LED2 afterwards

mode1:  out     PORTB, allLeds
        rcall   wait1
        out     PORTB, led1
        rcall   wait1
        out     PORTB, noLed
        rcall   wait1
        rcall   wait1
        rcall   wait1
        rjmp    main

mode2:  out     PORTB, allLeds
        rcall   wait1
        out     PORTB, led2
        rcall   wait1
        out     PORTB, noLed
        rcall   wait1
        rcall   wait1
        rcall   wait1
        rjmp    main

        ; 9'600'000 [Hz] /  8  / 254 /  99 /  2  /  2     ~ 12 [Hz]
        ; ___________      ___   ___   ___   ___   ___
        ;      |            |     |     |     |     |
        ;      |            |     |     |     |     +---------->  command execution time (~ 2 commands)
        ;      |            |     |     |     +--------------->  loop1
        ;      |            |     |     +-------------------->  loop2
        ;      |            |     +------------------------->  loop3
        ;      |            +------------------------------>  clock prescaler (see fuses)
        ;      +------------------------------------------>  clock frequency

wait1:  ldi     loop1, 3
w1:     dec     loop1
        breq    w4
        ldi     loop2, 100
w2:     dec     loop2
        breq    w1
        ldi     loop3, 255
w3:     dec     loop3
        brne    w3
        rjmp    w2
w4:     ret