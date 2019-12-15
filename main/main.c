//
//  main.c
//  Blink
//
//  Created by Michael Kwasnicki on 2018-02-09.
//  Copyright © 2018 Michael Kwasnicki. All rights reserved.
//


#include "config.h"
#include "macros.h"

#include "update.h"

#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>


#include <avr/eeprom.h>

uint8_t eeprom EEMEM = 129;


void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void main(void) __attribute__ ((OS_main)) __attribute__ ((section (".init9")));



void wdt_init(void) {
    wdt_reset(); // not doing this should be safe as it will be called in wdt_disable()
    MCUSR = 0x00;
    wdt_disable();
}



static void setup(void) {
    DDRB = _BV(PB5);
}



static void loop() {
    BIT_SET( PORTB, _BV(PB5));
    _delay_ms(10);
    BIT_CLR( PORTB, _BV(PB5));
    _delay_ms(1990);
}



void main(void) {
    setup();

    for (int i = 0; i < 10; i++) {
        loop();
    }

    // write();
    wdt_enable(WDTO_15MS);
    while (true);

//    do {
//        loop();
//    } while (true);
}

