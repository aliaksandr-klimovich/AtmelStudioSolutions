#include <avr/io.h>
#include <avr/interrupt.h>


#ifndef WS2812B_H
#define WS2812B_H

typedef struct __attribute__ ((__packed__)) {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_t;

/*
    Some description...

    r16 - temp
    r17 - bit mask
    r18 - red
    r19 - green
    r20 - blue
    r21 - saved status register
 */
#define WS2812B_CALL(PORT, PIN, P_BUF, BUF_SIZE) \
    __asm__ __volatile__ ( \
        /* preload variables */ \
        "0:     ldi     r17, 0x80               ; 1     \n" \
        "       ld      r18, %a[pBuf]+          ; 2     \n" \
        "       ld      r19, %a[pBuf]+          ; 2     \n" \
        "       ld      r20, %a[pBuf]+          ; 2     \n" \
        "       in      r21, __SREG__           ; 1     \n" \
        "       clt                             ; 1     \n" \
        "       cli                             ; 1     \n" \
        /* send green color first 7 bits */ \
        "1:     sbi     %[port], %[pin]         ; 2         2           \n" \
        "       nop                             ; 1         3           \n" \
        "       mov     r16, r17                ; 1         4           \n" \
        "       and     r16, r19                ; 1         5           \n" \
        "       brne    2f                      ; 1/2   6       7       \n" \
        "       cbi     %[port], %[pin]         ; 2     8       -       \n" \
        "       rjmp    3f                      ; 2     10      -       \n" \
        "2:     jmp     3f                      ; 3     -       10      \n" \
        "3:     jmp     4f                      ; 3         13          \n" \
        "4:     cbi     %[port], %[pin]         ; 2         15          \n" \
        "       nop                             ; 1         16          \n" \
        "       lsr     r17                     ; 1         17          \n" \
        "       cpi     r17, 1                  ; 1         18          \n" \
        "       brne    1b                      ; 1/2       20/19       \n" \
        "       nop                             ; 1         20          \n" \
        /* send green color last bit */ \
        "       sbi     %[port], %[pin]         ; 2         2           \n" \
        "       nop                             ; 1         3           \n" \
        "       mov     r16, r17                ; 1         4           \n" \
        "       and     r16, r19                ; 1         5           \n" \
        "       brne    5f                      ; 1/2   6       7       \n" \
        "       cbi     %[port], %[pin]         ; 2     8       -       \n" \
        "       rjmp    6f                      ; 2     10      -       \n" \
        "5:     jmp     6f                      ; 3     -       10      \n" \
        "6:     jmp     7f                      ; 3         13          \n" \
        "7:     cbi     %[port], %[pin]         ; 2         15          \n" \
        "       sbiw    %[bufSize], 1           ; 2         17          \n" \
        "       brne    8f                      ; 1/2       19/18       \n" \
        "       set                             ; 1         19          \n" \
        "8:     ldi     r17, 0x80               ; 1         20          \n" \
        /* send red color first 7 bits */ \
        "9:     sbi     %[port], %[pin]         ; 2         2           \n" \
        "       nop                             ; 1         3           \n" \
        "       mov     r16, r17                ; 1         4           \n" \
        "       and     r16, r18                ; 1         5           \n" \
        "       brne    10f                     ; 1/2   6       7       \n" \
        "       cbi     %[port], %[pin]         ; 2     8       -       \n" \
        "       rjmp    11f                     ; 2     10      -       \n" \
        "10:    jmp     11f                     ; 3     -       10      \n" \
        "11:    jmp     12f                     ; 3         13          \n" \
        "12:    cbi     %[port], %[pin]         ; 2         15          \n" \
        "       nop                             ; 1         16          \n" \
        "       lsr     r17                     ; 1         17          \n" \
        "       cpi     r17, 1                  ; 1         18          \n" \
        "       brne    9b                      ; 1/2       20/19       \n" \
        "       nop                             ; 1         20          \n" \
        /* send red color last bit */ \
        "       sbi     %[port], %[pin]         ; 2         2           \n" \
        "       nop                             ; 1         3           \n" \
        "       mov     r16, r17                ; 1         4           \n" \
        "       and     r16, r18                ; 1         5           \n" \
        "       brne    13f                     ; 1/2   6       7       \n" \
        "       cbi     %[port], %[pin]         ; 2     8       -       \n" \
        "       rjmp    14f                     ; 2     10      -       \n" \
        "13:    jmp     14f                     ; 3     -       10      \n" \
        "14:    jmp     15f                     ; 3         13          \n" \
        "15:    cbi     %[port], %[pin]         ; 2         15          \n" \
        "       ld      r18, %a[pBuf]+          ; 2         17          \n" \
        "       ld      r19, %a[pBuf]+          ; 2         19          \n" \
        "       ldi     r17, 0x80               ; 1         20          \n" \
        /* send blue color first 7 bits */ \
        "16:    sbi     %[port], %[pin]         ; 2         2           \n" \
        "       nop                             ; 1         3           \n" \
        "       mov     r16, r17                ; 1         4           \n" \
        "       and     r16, r20                ; 1         5           \n" \
        "       brne    17f                     ; 1/2   6       7       \n" \
        "       cbi     %[port], %[pin]         ; 2     8       -       \n" \
        "       rjmp    18f                     ; 2     10      -       \n" \
        "17:    jmp     18f                     ; 3     -       10      \n" \
        "18:    jmp     19f                     ; 3         13          \n" \
        "19:    cbi     %[port], %[pin]         ; 2         15          \n" \
        "       nop                             ; 1         16          \n" \
        "       lsr     r17                     ; 1         17          \n" \
        "       cpi     r17, 1                  ; 1         18          \n" \
        "       brne    16b                     ; 1/2       20/19       \n" \
        "       nop                             ; 1         20          \n" \
        /* send blue color last bit */ \
        "       sbi     %[port], %[pin]         ; 2         2           \n" \
        "       nop                             ; 1         3           \n" \
        "       mov     r16, r17                ; 1         4           \n" \
        "       and     r16, r20                ; 1         5           \n" \
        "       brne    20f                     ; 1/2   6       7       \n" \
        "       cbi     %[port], %[pin]         ; 2     8       -       \n" \
        "       rjmp    21f                     ; 2     10      -       \n" \
        "20:    jmp     21f                     ; 3     -       10      \n" \
        "21:    ld      r20, %a[pBuf]+          ; 2         12          \n" \
        "       ldi     r17, 0x80               ; 1         13          \n" \
        "       cbi     %[port], %[pin]         ; 2         15          \n" \
        "       nop                             ; 1         16          \n" \
        /* cycle */ \
        "       brts    22f                     ; 1/2       17/18       \n" \
        "       jmp     1b                      ; 3         20          \n" \
        "       ; sbi ?                         ; 2                     \n" \
        "22:    out     __SREG__, r21           ; 1                     \n" \
        : \
        : [port]    "I" (_SFR_IO_ADDR(PORT)), \
          [pin]     "I" (PIN), \
          [pBuf]    "z" (P_BUF), \
          [bufSize] "w" (BUF_SIZE) \
        : "r16", "r17", "r18", "r19", "r20", "r21" \
    )

#define WS2812B_FN(NAME, PORT, PIN) \
extern void NAME(const RGB_t *rgb, uint16_t bufSize) __attribute__((noinline)); \
void NAME(const RGB_t *rgb, uint16_t bufSize) { WS2812B_CALL(PORT, PIN, rgb, bufSize); }

#endif /* WS2812B_H */



#define LED_DDR     DDRB
#define LED_PORT    PORTB
#define LED_PIN     PB0

#define LED_COUNT 2

WS2812B_FN(WS2812B_A, LED_PORT, LED_PIN);
RGB_t rgb[LED_COUNT];

int main(void)
{
    // Disable interrupts if any
    cli();

    // Disable analog comparator
    ACSR = (1 << ACD);

    // Setup DDR and PORT on pin PB0
    LED_DDR |= (1 << LED_PIN);  // as output
    LED_PORT &= ~(1 << LED_PIN);  // low

    //rgb[0] = (RGB_t){0, 0, 0};
    //rgb[0] = (RGB_t){0xFF, 0xFF, 0xFF};
    rgb[0] = (RGB_t){0x55, 0x55, 0x55};
    rgb[1] = (RGB_t){0xAA, 0xAA, 0xAA};
    //rgb[0] = (RGB_t){1, 3, 2};

    // write to buffer
    WS2812B_A((const RGB_t *) rgb, LED_COUNT);

    while (1) {}
}
