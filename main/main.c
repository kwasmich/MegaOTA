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



uint8_t eeprom EEMEM = 129;



void main(void) __attribute__((OS_main, section (".init9")));



static void setup() {
    DDRB = _BV(PB5);

    for (int i = 0; i < 10; i++) {
        BIT_SET( PORTB, _BV(PB5));
        _delay_ms(10);
        BIT_CLR( PORTB, _BV(PB5));
        _delay_ms(490);
    }
}



static void loop() {
    BIT_SET( PORTB, _BV(PB5));
    _delay_ms(10);
    BIT_CLR( PORTB, _BV(PB5));
    _delay_ms(1990);
}



void main(void) {
    // set all ports to output and low to reduce power consumption
    // DDRB = 0xFF;
    // DDRC = 0x7F;
    // DDRD = 0xFF;
    // PORTB = 0x00;
    // PORTC = 0x00;
    // PORTD = 0x00;
    setup();

    for (int i = 0; i < 5; i++) {
        loop();
    }

//    write();
    wdt_enable(WDTO_15MS);
    while (true);

//    do {
//        loop();
//    } while (true);
}
