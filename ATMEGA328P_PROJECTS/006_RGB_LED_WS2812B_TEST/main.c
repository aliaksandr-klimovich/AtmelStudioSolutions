#include <avr/io.h>
#include <avr/interrupt.h>

#define DEBUG

// LED strip. 60 LEDs per meter, 5 meters.
#define LED_COUNT   (60U * 5U)

// LED count * 3 coded colors (Red, Green, Blue)
#ifndef DEBUG
#define BUF_SIZE  (LED_COUNT * 3U)
#else
#define BUF_SIZE  3
#endif

// Buffer with next format (based on WS2812B datasheet):
// (RGB color is encoded in next format, 24 bits (3 bytes), 8 bits per color, starting from green)
// G7 G6 G5 G4 G3 G2 G1 G0 R7 R6 R5 R4 R3 R2 R1 R0 B7 B6 B5 B4 B3 B2 B1 B0
// It is multiplied by LED_COUNT and results a buffer size.
uint8_t buf[BUF_SIZE];  // 60 * 5 * 3 = 900 bytes

#define LED_DDR     DDRB
#define LED_PORT    PORTB
#define LED_PIN     PB0

void WS2812B_write(uint8_t *pBuf /*uint8_t port, uint8_t pin*/ )
{
    uint16_t i = BUF_SIZE;  // buffer indexing is not used, left bytes count is used instead
    uint8_t byte;           // one byte read out from the buffer
    uint8_t bitMask;        // bit mask to check each bit
    uint8_t temp;           // result for bitwise operation: temp = (byte & mask)

    // code is compatible only with 16M clk
    asm volatile (                               // cmd time in ticks : total ticks if bit is 0 : total ticks if bit is 1; comment
        "   ld      %[byte], %a[pBuf]+      \n"  // 2 :: ; new placement (optimization); byte from the buffer is loaded, buffer pointer is post-incremented
        "   ldi     %[bitMask], 0x80        \n"  // 1 :: ; new placement (optimization); most left bit in the mask is set 
        "WS2812B_write_0:                   \n"
        //"   ld      %[byte], %a[pBuf]+      \n"  // 2 :: ; original placement
        //"   ldi     %[bitMask], 0x80        \n"  // 1 :: ; original placement
        "WS2812B_write_1:                   \n"    
        "   sbi     %[port], %[pin]         \n"  // 2    : 2   : 2  ; regardless the bit status pin is set to high
        "   nop                             \n"  // 1    : 3   : 3 
        "   mov     %[temp], %[bitMask]     \n"  // 1    : 4   : 4  ; bit mask is saved to a temporary register
        "   and     %[temp], %[byte]        \n"  // 1    : 5   : 5  ; bit mask is applied on a byte, result is saved to temp
        "   brne    WS2812B_write_2         \n"  // 1/2  : 6   : 7  ; if bit is set, jump to label 2 is made
        "   cbi     %[port], %[pin]         \n"  // 2    : 8   : -  ; bit is 0, pin is set to low and wait
        "   rjmp    WS2812B_write_20        \n"  // 2    : 10  : -  ; time adjustment
        "WS2812B_write_2:                   \n"
        "   nop                             \n"  // 1    : -   : 8  ; bit is 1, wait
        "   nop                             \n"  // 1    : -   : 9
        "   nop                             \n"  // 1    : -   : 10
        "WS2812B_write_20:                  \n"  // time adjustment
        "   nop                             \n"  // 1    : 11    : 11
        "   nop                             \n"  // 1    : 12    : 12
        "   nop                             \n"  // 1    : 13    : 13
        "   cbi     %[port], %[pin]         \n"  // 2    : 15    : 15 ; regardless the bit status pin is set to low
        "   nop                             \n"  // 1    : 16    : 16
        "   lsr     %[bitMask]              \n"  // 1    : 17    : 17 ; logical shift bit mask to the right
        "   cpi     %[bitMask], 1           \n"  // 1    : 18    : 18 ; compare bit mask with 1 to process last bit separately
        "   brne    WS2812B_write_1         \n"  // 1/2  : 20/19 : 20/19 ; process first 7 bits, then process last bit
        "   nop                             \n"  // 1    : 20    : 20 
        "   sbi     %[port], %[pin]         \n"  // 2    : 22    : 22 ; regardless the bit status pin is set to high
        "   nop                             \n"  // 1    : 23    : 23 
        "   mov     %[temp], %[bitMask]     \n"  // 1    : 24    : 24 ; bit mask is saved to a temporary register
        "   and     %[temp], %[byte]        \n"  // 1    : 25    : 25 ; bit mask is applied on a byte, result is saved to temp
        "   brne    WS2812B_write_3         \n"  // 1/2  : 26/27 : 26/27 ; if bit is set, jump to label 3 is made
        "   cbi     %[port], %[pin]         \n"  // 2    : 28    : -     ; bit is 0, pin is set to low and wait
        "   rjmp    WS2812B_write_30        \n"  // 2    : 30    : -     ; time adjustment
        "WS2812B_write_3:                   \n"
        "   nop                             \n"  // 1    : -  : 28
        "   nop                             \n"  // 1    : -  : 29
        "   nop                             \n"  // 1    : -  : 30
        "WS2812B_write_30:                  \n"  // time adjustment
        "   nop                             \n"  // 1    : 31 : 31
        "   ld      %[byte], %a[pBuf]+      \n"  // 2    : 33 : 33 ; new placement (optimization); this instruction is moved from the top because of the time constraints, originally nop was here
        "   cbi     %[port], %[pin]         \n"  // 2    : 35 : 35 ; regardless the bit status pin is set to low
        "   ldi     %[bitMask], 0x80        \n"  // 1    : 36 : 36 ; new placement (optimization); this instruction is moved from the top because of the time constraints, originally nop was here
        "   sbiw    %[i], 1                 \n"  // 2    : 38 : 38 ; decrease buffer left counter
        "   brne    WS2812B_write_0         \n"  // 1/2  : 40 : 40 ; near jump; get next byte from the buffer or finish buffer processing
        
        // output parameters:
        : [i]"+w" (i), [pBuf]"+e"(pBuf), [byte]"+r"(byte), [bitMask]"+r"(bitMask), [temp]"+r"(temp)
        
        // input parameters
        : [port]"I" (_SFR_IO_ADDR(LED_PORT)), [pin]"I" (LED_PIN)
        
        // clobbers
        :
    );
}

int main(void)
{
    // Disable interrupts if any
    cli();

    // Disable analog comparator
    ACSR = _BV(ACD);

    // Setup DDR and PORT on pin PB0
    LED_DDR |= _BV(LED_PIN);  // as output
    LED_PORT &= ~_BV(LED_PIN);  // low

#ifdef DEBUG
    buf[0] = 0b01000010;
    buf[1] = 0b11111101;
    buf[2] = 0b00000000;
#endif

    // write to buffer
    WS2812B_write(buf);

    while (1) {}
}
