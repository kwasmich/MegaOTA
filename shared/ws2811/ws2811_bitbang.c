//
//  ws2811.c
//  MegaNeoPixel
//
//  Created by Michael Kwasnicki on 20.07.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#include "ws2811.h"

#include "macros.h"

#include <avr/io.h>
#include <util/delay.h>



#if F_CPU > 8000000UL
#   warning WS2811 is intended for 8MHz MCUs only
#endif



void ws2811_send_bitbang(const uint8_t *values, uint8_t size, uint8_t bit) {
    uint8_t bitcount = 6;
    const uint8_t dn = WS2811_PORT & (~_BV(bit));
    const uint8_t up = WS2811_PORT | _BV(bit);





    __asm__ volatile(
                 "        LD __tmp_reg__, %a[dataptr]+      \n"
                 "        LDI %[bits], 0x6                  \n"
                 "bit0:   OUT %[portout], %[rDN]            \n"
                 "bit1:   LSL __tmp_reg__                   \n"
                 "        OUT %[portout], %[rUP]            \n"
                 "        BRCS one1                         \n"
                 "        OUT %[portout], %[rDN]            \n"
                 "        RJMP .+0              ; NOP NOP   \n"
                 "        DEC %[bits]                       \n"
                 "        BREQ bit7a                        \n"
                 "        RJMP bit1                         \n"
                 "one1:   RJMP .+0              ; NOP NOP   \n"
                 "        DEC %[bits]                       \n"
                 "        BRNE bit0                         \n"
                 "        NOP                               \n"
                 "bit7a:  OUT %[portout], %[rDN]            \n"
                 "bit7:   LSL __tmp_reg__                   \n"
                 "        OUT %[portout], %[rUP]            \n"
                 "        BRCS one7                         \n"
                 "        OUT %[portout], %[rDN]            \n"
                 "        RJMP .+0              ; NOP NOP   \n"
                 "        LDI %[bits], 0x6                  \n"
                 "        NOP                               \n"
                 "        RJMP bit8                         \n"
                 "one7:   RJMP .+0              ; NOP NOP   \n"
                 "        LDI %[bits], 0x6                  \n"
                 "        RJMP .+0              ; NOP NOP   \n"
                 "        OUT %[portout], %[rDN]            \n"
                 "bit8:   LSL __tmp_reg__                   \n"
                 "        OUT %[portout], %[rUP]            \n"
                 "        BRCS one8                         \n"
                 "        OUT %[portout], %[rDN]            \n"
                 "        LD __tmp_reg__, %a[dataptr]+      \n"
                 "        DEC %[rSize]                      \n"
                 "        BREQ end                          \n"
                 "        RJMP bit1                         \n"
                 "one8:   LD __tmp_reg__, %a[dataptr]+      \n"
                 "        DEC %[rSize]                      \n"
                 "        BRNE bit0                         \n"
                 "        NOP                               \n"
                 "        OUT %[portout], %[rDN]            \n"
                 "end:                                      \n"
                 : /* no output */
                 : /* inputs */
                 [dataptr] "e" (values),                    // rgb color values
                 [rUP]     "r" (up),                        // cached register state for hi
                 [rDN]     "r" (dn),                        // cached register state for lo
                 [rSize]   "d" (size),                      // number of bytes to send
                 [bits]    "d" (bitcount),                  // track number of processed bits
                 [portout] "I" (_SFR_IO_ADDR(WS2811_PORT))  // The port to use
                 );

    _delay_loop_2(1000);    // reset delay is 0.5ms = 1000 * CPU cycles @ 8MHz
}
