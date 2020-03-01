//
//  main.c
//  Blink
//
//  Created by Michael Kwasnicki on 2018-02-09.
//  Copyright © 2018 Michael Kwasnicki. All rights reserved.
//


#include "config.h"
#include "macros.h"

#include "debug.h"
#include "update.h"

#include "ihex/ihex.h"
#include "spi/spi.h"
#include "uart/uart.h"
#include "nrf24/nrf24.h"



#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>










uint8_t eeprom EEMEM = 129;



void main(void) __attribute__((OS_main, section(".init9")));



static void setup() {
    DDRB = _BV(PB5);

    spi_init();
    nrf24_init();
    uart_init_async(0x00);
    sei();
    puts("ready");
}



static void loop() {
    static ihex_state_t ihex;
    static update_page_t ublock;
    char c;
    static const uint8_t payload[32] = "Hello World!";

    if (uart_getchar_async(&c)) {
        if (ihex_parse_async(&ihex, c)) {
            if (ihex.chksum_valid) {
                switch (ihex.type) {
                    case 0x00:
                        update_page_add(&ublock, ihex.len, ihex.data, ihex.offset);

                        printf("base: %04x\n", ublock.base_address);

                        for (int i = 0; i < SPM_PAGESIZE; i++) {
                            printf("%02x ", ublock.data[i]);

                            if ((i & 0xF) == 0xF) {
                                puts("");
                            }
                        }

                        break;

                    case 0x06:
                        puts("writing to PROGMEM...");
                        update_write_page(&ublock);
                        puts("DONE");
                        break;

                    case 0x07:
                        debug_dump_mem();
                        break;

                    case 0x08:
                        debug_dump_eep();
                        break;

                    case 0x09:
                        debug_dump_pgm();
                        break;

                    case 0x0A:
                        nrf24_debug();
                        nrf24_tx(12, payload);
                        nrf24_debug();
                        break;
                }

                puts("\ndone");

                puts("");
                printf("%02x = %d\n", ihex.len, ihex.len);
                printf("%04x = %d\n", ihex.offset, ihex.offset);
                printf("%02x = %d\n", ihex.type, ihex.type);

                for (uint8_t i = 0; i < ihex.len; i++) {
                    printf("%02d: 0x%0x\n", i, ihex.data[i]);
                }

                printf("%02x = %d\n", ihex.chksum, ihex.chksum);
                printf("%02x = %d\n", ihex.chksum_valid, ihex.chksum_valid);
            } else {
                puts("error!");
            }
        }
        if (c == 'r') {
            puts("start listening!");
            nrf24_rx_start();
        }
        if (c == 's') {
            nrf24_rx_stop();
            puts("stopped listening!");
        }
        if (c == 'd') {
            nrf24_debug();
        }
        if (c == 't') {
            nrf24_tx(12, payload);
        }
        if (c == 'z') {
            BIT_SET(PORTB, _BV(PB1));
            _delay_us(15);
            BIT_CLR(PORTB, _BV(PB1));
        }
        if (c == 'u') {
            BIT_SET(PORTB, _BV(PB1));
        }
        if (c == 'i') {
            BIT_CLR(PORTB, _BV(PB1));
        }
        if (c == 'a') {
            BIT_CLR(PORTB, _BV(PB2));
            spi_exchange(0xA0);
            BIT_SET(PORTB, _BV(PB2));
        }
    }

//    BIT_SET(PORTB, _BV(PB5));
//    _delay_ms(10);
//    BIT_CLR(PORTB, _BV(PB5));
//    _delay_ms(1990);

    // puts("blink");

//    if (s_uart_buffer_ready) {
//        cli();
//        puts(s_uart_buffer);
//        s_uart_buffer_fill = 0;
//        s_uart_buffer[0] = 0;
//        s_uart_buffer_ready = false;
//        sei();
//    }
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

    do {
        loop();
    } while (true);

//    write();
//    wdt_enable(WDTO_15MS);
//    while (true);
}
