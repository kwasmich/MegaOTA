//
//  entropy.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 13.04.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "entropy.h"

#include <avr/interrupt.h>
#include <avr/wdt.h>



static uint8_t rotl(const uint8_t value) {
  return (value << 1) | (value >> 7);
}


// Using jitter in WDT to generate random sequence
// TODO: Timer/Counter 1 must be active
void entropy(uint8_t len, uint8_t arr[static const len]) {
  static uint8_t s_random = 0;
  uint8_t sreg = SREG;
  uint8_t wdtcsr = WDTCSR;
  cli();
  wdt_reset();
  WDTCSR |= _BV(WDCE) | _BV(WDE);
  WDTCSR = _BV(WDIE);

  while (len) {
    loop_until_bit_is_set(WDTCSR, WDIF);
    WDTCSR |= _BV(WDIF);
    s_random = rotl(s_random);                                                  // Spread randomness around
    s_random ^= TCNT1L;                                                         // XOR preserves randomness
    arr[--len] = s_random;
  }

  WDTCSR |= _BV(WDCE) | _BV(WDE);
  WDTCSR = wdtcsr;
  SREG = sreg;
}
