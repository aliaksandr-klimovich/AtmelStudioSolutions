#ifndef WS2812B_H
#define WS2812B_H

// To use _SFR_IO_ADDR function
#include <avr/sfr_defs.h>

// Structure definition to use with this driver
typedef struct __attribute__ ((__packed__)) {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_t;

/*
    WS2812B driver in a macro implementation.
    
    It is based on the next needs:
    - Possibility to use on any I/O pin.
    - RBG structure can be passed to the driver function, not GRB as defined in the ref.man. driver.
    
    Restrictions:
    - 16 MHz clock

    Next register are used:
    r16 - temporary
    r17 - bit mask for one color (byte)
    r18 - red
    r19 - green
    r20 - blue
    r21 - saved status register
 */
#define WS2812B_MACRO(PORT, PIN, P_BUF, BUF_SIZE) \
    __asm__ __volatile__ ( \
        /* preload variables */ \
        "0:     ldi     r17, 0x80               ; 1     Most left bit is set in the bit mask register \n" \
        "       ld      r18, %a[pBuf]+          ; 2     Red, green and blue colors are loaded from the buffer to registers.\n" \
        "       ld      r19, %a[pBuf]+          ; 2       Later on they will be loaded my need, but not by this code \n" \
        "       ld      r20, %a[pBuf]+          ; 2       (that starts from label 0). \n" \
        "       in      r21, __SREG__           ; 1     Status register is saved.\n" \
        "       clt                             ; 1     T flag is cleared. T flag indicates that the buffer has run out. \n" \
        "       cli                             ; 1     I flag is cleared. This disables all interrupts. \n" \
        /* send green color first 7 bits */ \
        ";                          Numbers are tick for: current instruction,  0 transmission, 1 transmission \n" \
        ";                            or same value for 0 and 1 transmission (value in the middle) \n" \
        ";                            accumulated value for one bit transmission. \n" \
        ";                          For example: sbi takes 2 ticks to run, 0+2=2 for 0 and 1 transmission, \n" \
        ";                            then nop takes 1 tick, 0+2+1=3 or simply adding 1 to previous value 2+1=3, \n" \
        ";                            brne takes 1 tick if Z flag is set and 2 ticks if Z == 0 (in case if jump) - \n" \
        ";                              in this case 5+1=6 for 0 transmission and 5+2=7 for 1 transmission. \n" \
        ";                          In case if other branches, first value shows the first most probable way of the scenario. \n" \
        "; \n" \
        "1:     sbi     %[port], %[pin]         ; 2         2           Set pin to HIGH \n" \
        "       nop                             ; 1         3           No OPeration \n" \
        "       mov     r16, r17                ; 1         4           Save bit mask \n" \
        "       and     r16, r19                ; 1         5           Apply bit mask on the green byte \n" \
        "       brne    2f                      ; 1/2   6       7       Branch if 0 bit in green byte was found \n" \
        "       cbi     %[port], %[pin]         ; 2     8       -       Set pin to LOW \n" \
        "       rjmp    3f                      ; 2     10      -       Time adjustment \n" \
        "2:     jmp     3f                      ; 3     -       10      Optimization (to replace nop-s) \n" \
        "3:     jmp     4f                      ; 3         13          Optimization (to replace nop-s) \n" \
        "4:     cbi     %[port], %[pin]         ; 2         15          Set pin to LOW \n" \
        "       nop                             ; 1         16          No operation \n" \
        "       lsr     r17                     ; 1         17          Shift bit mask to the right \n" \
        "       cpi     r17, 1                  ; 1         18          Check is bit mask is 1 (last bit left) \n" \
        "       brne    1b                      ; 1/2       20/19       Branch if not the last bit \n" \
        "       nop                             ; 1         20          No operation \n" \
        /* send green color last bit */ \
        "       sbi     %[port], %[pin]         ; 2         2           Set pin to HIGH \n" \
        "       nop                             ; 1         3           ... no more repeated comments \n" \
        "       mov     r16, r17                ; 1         4           \n" \
        "       and     r16, r19                ; 1         5           \n" \
        "       brne    5f                      ; 1/2   6       7       \n" \
        "       cbi     %[port], %[pin]         ; 2     8       -       \n" \
        "       rjmp    6f                      ; 2     10      -       \n" \
        "5:     jmp     6f                      ; 3     -       10      \n" \
        "6:     jmp     7f                      ; 3         13          \n" \
        "7:     cbi     %[port], %[pin]         ; 2         15          \n" \
        "       sbiw    %[bufSize], 1           ; 2         17          Decrease buffer size by 1 \n" \
        "       brne    8f                      ; 1/2       19/18       Check for zero \n" \
        "       set                             ; 1         19          If run out of buffer - set T flag \n" \
        "8:     ldi     r17, 0x80               ; 1         20          \n" \
        /* send red color first 7 bits */ \
        "9:     sbi     %[port], %[pin]         ; 2         2           \n" \
        "       nop                             ; 1         3           \n" \
        "       mov     r16, r17                ; 1         4           \n" \
        "       and     r16, r18                ; 1         5           Check red byte \n" \
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
        "       ld      r18, %a[pBuf]+          ; 2         17          Read red \n" \
        "       ld      r19, %a[pBuf]+          ; 2         19          and green colors (bytes) \n" \
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
        "21:    ld      r20, %a[pBuf]+          ; 2         12          Read blue color (byte) \n" \
        "       ldi     r17, 0x80               ; 1         13          \n" \
        "       cbi     %[port], %[pin]         ; 2         15          \n" \
        "       nop                             ; 1         16          \n" \
        /* cycle */ \
        "       brts    22f                     ; 1/2       17/18       If T flag is set (all the buffer was sent) - jump to finish \n" \
        "       jmp     1b                      ; 3         20          Otherwise jump to start (label 1) \n" \
        "22:    ;rjmp    23f                     ; 2         20          Currently it is unclear if there is a need to set to HIGH (and then to LOW) \n" \
        "23:    ;sbi     %[port], %[pin]         ; 2                       a pin at the end as the 1 bit transmission should end with HIGH state,\n" \
        "       ;cbi     %[port], %[pin]         ; 2         15            but LOW -> HIGH transition initiate the transmission. \n" \
        "       out     __SREG__, r21           ; 1                     Status register is restored \n" \
        : /* no output parameters */ \
        : /* input parameters */ \
          [port]    "I" (_SFR_IO_ADDR(PORT)), \
          [pin]     "I" (PIN), \
          [pBuf]    "z" (P_BUF), \
          [bufSize] "w" (BUF_SIZE) \
        : /* clobbers */ \
          "r16", "r17", "r18", "r19", "r20", "r21" /* no "cc", no "memory" */ \
    )

/*
    Macro to create a function to use with the desired PORT and PIN
    For example: 
        WS2812B_CREATE_FUNCTION(WS2812B_PB0, PORTB, PB0);
        // WS2812B_PB0 is the function name that will be available, you can name it as you wish.
        
    Once the function is create, you can use it passing buffer pointer and buffer size variables.
    For example:
        #define BUF_SIZE 60
        RGB_t buf[BUF_SIZE];
        // some interactions with a buffer
        WS2812B_PB0(buf, BUF_SIZE);
*/
#define WS2812B_CREATE_FUNCTION(FUNCTION_NAME, PORT, PIN) \
extern void FUNCTION_NAME(const RGB_t *pBuf, uint16_t bufSize) __attribute__ ((noinline)); \
void FUNCTION_NAME(const RGB_t *pBuf, uint16_t bufSize) { WS2812B_MACRO(PORT, PIN, pBuf, bufSize); }

#endif /* WS2812B_H */