//
//  entropy.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 13.04.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

// inspired by https://gist.github.com/endolith/2568571
// tested with https://hackaday.com/2017/11/02/what-is-entropy-and-how-do-i-get-more-of-it/


#include "entropy.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/atomic.h>



static uint8_t rotl(const uint8_t value) {
    return (value << 1) | (value >> 7);
}


// Using jitter in WDT to generate random sequence
void entropy(uint8_t len, uint8_t arr[static const len]) {
    static uint8_t s_random = 0;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        uint8_t prr = PRR;                                                          // store power reduction settings
        power_timer1_enable();                                                      // enable TC1
        uint8_t tccr1a = TCCR1A;                                                    // store TC1 settings
        uint8_t tccr1b = TCCR1B;
        TCCR1A = 0x0;                                                               // stop and reset TC1 settings
        TCCR1B = 0x0;
        uint16_t tcnt1 = TCNT1;                                                     // store current TC1 value
        TCCR1B = 0x1;                                                               // run TC1 at full speed
        uint8_t wdtcsr = WDTCSR;                                                    // store Watchdog settings
        wdt_reset();
        WDTCSR |= _BV(WDCE) | _BV(WDE);
        WDTCSR = _BV(WDIE);

        while (len) {
            loop_until_bit_is_set(WDTCSR, WDIF);
            WDTCSR |= _BV(WDIF);
            s_random = rotl(s_random);                                              // Spread randomness around
            s_random ^= TCNT1L;                                                     // XOR preserves randomness
            arr[--len] = s_random;
        }

        wdt_reset();
        WDTCSR |= _BV(WDCE) | _BV(WDE);
        WDTCSR = wdtcsr;                                                            // restore Watchdog settings
        TCCR1B = 0x0;                                                               // stop and reset TC1 settings
        TCNT1 = tcnt1;                                                              // restore TC1 value
        TCCR1A = tccr1a;                                                            // restore TC1 settings
        TCCR1B = tccr1b;
        PRR = prr;                                                                  // restore power reduction settings
    }
}
