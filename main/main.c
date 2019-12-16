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


//void wdt_init(void) __attribute__((naked, section(".init3")));
void main(void) __attribute__((OS_main, section (".init9")));
//void boot0(void) __attribute__ ((OS_main)) __attribute__ ((section (".boot0")));
//void boot1(void) __attribute__ ((OS_main)) __attribute__ ((section (".boot1")));
//void boot2(void) __attribute__ ((OS_main)) __attribute__ ((section (".boot2")));
//void boot3(void) __attribute__ ((OS_main)) __attribute__ ((section (".boot3")));
//
//const uint8_t __attribute__((section(".boot0_guard"))) boot0_version = 0xff;
//const uint8_t __attribute__((section(".boot1_guard"))) boot1_version = 0xfe;
//const uint8_t __attribute__((section(".boot2_guard"))) boot2_version = 0xfd;
//const uint8_t __attribute__((section(".boot3_guard"))) boot3_version = 0xfc;



//void wdt_init(void) {
//    wdt_reset(); // not doing this should be safe as it will be called in wdt_disable()
//    MCUSR = 0x00;
//    wdt_disable();
//}



static void setup(void) {
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

/*

// 32 pages
void boot0(void) {
    DDRB = _BV(PB5);

    BIT_TGL(PORTB, _BV(PB5));
    _delay_ms(500);

    for (uint8_t i = 0; i < 15; i++) {
        BIT_TGL(PORTB, _BV(PB5));
        _delay_ms(50);
    }

    _delay_ms(450);
    BIT_TGL(PORTB, _BV(PB5));
    _delay_ms(500);

    asm("jmp 0x0000");
}



// 16 pages
void boot1(void) {
    wdt_reset(); // not doing this should be safe as it will be called in wdt_disable()
    MCUSR = 0x00;
    wdt_disable();
    
    DDRB = _BV(PB5);

    BIT_TGL(PORTB, _BV(PB5));
    _delay_ms(500);

    for (uint8_t i = 0; i < 63; i++) {
        BIT_TGL(PORTB, _BV(PB5));
        _delay_ms(50);
    }

    _delay_ms(450);
    BIT_TGL(PORTB, _BV(PB5));
    _delay_ms(500);

    asm("jmp 0x0000");
}



// 8 pages
void boot2(void) {
    DDRB = _BV(PB5);

    BIT_TGL(PORTB, _BV(PB5));
    _delay_ms(500);

    for (uint8_t i = 0; i < 127; i++) {
        BIT_TGL(PORTB, _BV(PB5));
        _delay_ms(50);
    }

    _delay_ms(450);
    BIT_TGL(PORTB, _BV(PB5));
    _delay_ms(500);

    asm("jmp 0x0000");
}



// 4 pages
void boot3(void) {
    DDRB = _BV(PB5);

    BIT_TGL(PORTB, _BV(PB5));
    _delay_ms(500);

    for (uint8_t i = 0; i < 255; i++) {
        BIT_TGL(PORTB, _BV(PB5));
        _delay_ms(50);
    }

    _delay_ms(450);
    BIT_TGL(PORTB, _BV(PB5));
    _delay_ms(500);

    asm("jmp 0x0000");
}

*/
