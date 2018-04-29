//
//  main.c
//  Blink
//
//  Created by Michael Kwasnicki on 2018-02-09.
//  Copyright © 2018 Michael Kwasnicki. All rights reserved.
//


#include "config.h"
#include "macros.h"


#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>


void main(void) __attribute__ ((OS_main)) __attribute__ ((section (".init9")));


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

    do {
        loop();
    } while (true);
}

