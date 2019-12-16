#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_COUNT   (60U * 5U)

//#define BUF_SIZE  (LED_COUNT * 3U)
#define BUF_SIZE  3
uint8_t buf[BUF_SIZE];  // 60 * 5 * 3 = 900 bytes

#define LED_DDR     DDRB
#define LED_PORT    PORTB
#define LED_PIN     PB0

void WS2812B_write(uint8_t *pBuf)
{
    uint16_t i = BUF_SIZE;
    uint8_t byte;
    uint8_t bitMask;
    uint8_t temp;

    // code is compatible only with 16M clk
    asm volatile (

        "   ld      %[byte], %a[pBuf]+      \n"  // 2t; new placement (optimization)
        "   ldi     %[bitMask], 0x80        \n"  // 1t; new placement (optimization)
        "WS2812B_write_0:                   \n"
        //"   ld      %[byte], %a[pBuf]+      \n"  // original placement
        //"   ldi     %[bitMask], 0x80        \n"  // original placement
        "WS2812B_write_1:                   \n"  // 0t   "0"   "1"
        "   sbi     %[port], %[pin]         \n"  // 2t   :2t   :2t
        "   nop                             \n"  // 1t   :3t   :3t
        "   mov     %[temp], %[bitMask]     \n"  // 1t   :4t   :4t
        "   and     %[temp], %[byte]        \n"  // 1t   :5t   :5t
        "   brne    WS2812B_write_2         \n"  // 1/2t :6t   :7t
        "   cbi     %[port], %[pin]         \n"  // 2t   :8t   :
        "   rjmp    WS2812B_write_20        \n"  // 2t   :10t; time adjustment
        "WS2812B_write_2:                   \n"
        "   nop                             \n"  // 1t  :     :8t
        "   nop                             \n"  // 1t  :     :9t
        "   nop                             \n"  // 1t  :     :10t
        "WS2812B_write_20:                  \n"  // time adjustment
        "   nop                             \n"  // 1t   :11t  :11t
        "   nop                             \n"  // 1t   :12t  :12t
        "   nop                             \n"  // 1t   :13t  :13t
        "   cbi     %[port], %[pin]         \n"  // 2t   :15t  :15t
        "   nop                             \n"  // 1t   :16t  :16t
        "   lsr     %[bitMask]              \n"  // 1t   :17t  :17t
        "   cpi     %[bitMask], 1           \n"  // 1t   :18t  :18t
        "   brne    WS2812B_write_1         \n"  // 1/2t :20t/19t  :20t/19t
        "   nop                             \n"  // 1t   :
        "   sbi     %[port], %[pin]         \n"  // 2t
        "   nop                             \n"  // 1t
        "   mov     %[temp], %[bitMask]     \n"  // 1t
        "   and     %[temp], %[byte]        \n"  // 1t
        "   brne    WS2812B_write_3         \n"  // 1t
        "   cbi     %[port], %[pin]         \n"  // 2t
        "   rjmp    WS2812B_write_30        \n"  // time adjustment
        "WS2812B_write_3:                   \n"
        "   nop                             \n"  // 1t
        "   nop                             \n"  // 1t
        "   nop                             \n"  // 1t
        "WS2812B_write_30:                  \n"  // time adjustment
        "   nop                             \n"  // 1t
        "   ld      %[byte], %a[pBuf]+      \n"  // 2t; new placement (optimization)
        "   cbi     %[port], %[pin]         \n"  // 2t
        "   ldi     %[bitMask], 0x80        \n"  // 1t; new placement (optimization)
        "   sbiw    %[i], 1                 \n"  // 2t
        "   brne    WS2812B_write_0         \n"  // 1t

        : [i]"+w" (i), [pBuf]"+e"(pBuf), [byte]"+r"(byte), [bitMask]"+r"(bitMask), [temp]"+r"(temp)
        : [port]"I" (_SFR_IO_ADDR(LED_PORT)), [pin]"I" (LED_PIN)
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

    // test bits
    buf[0] = 0b01000010;
    buf[1] = 0b11111101;
    buf[2] = 0b00000000;

    // write to buffer
    WS2812B_write(buf);

    while (1)
    {

    }
}

