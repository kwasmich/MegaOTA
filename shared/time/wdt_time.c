//
//  wdt_time.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 17.07.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "wdt_time.h"

#include "config.h"
#include "macros.h"

#include "time/time.h"

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdbool.h>


#ifdef TIME_WDT

ISR(WDT_vect) {
    static bool odd = false;
    s_time_ms += 16 + odd;
    odd = !odd;
}



void time_wdt_init() {
    cli();
    wdt_reset();
    BIT_SET(WDTCSR, _BV2(WDCE, WDE));
    WDTCSR = 0;
    BIT_SET(WDTCSR, _BV(WDIE));
    sei();
}

#endif // TIME_WDT
