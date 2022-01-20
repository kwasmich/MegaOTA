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
#include "nrf24/nrf24_io.h"
#include "nrf24/nrf24_coordinator.h"
#include "nrf24/nrf24_router.h"
#include "lcd/lcd.h"
#include "crypto/entropy.h"
#include "time/time.h"


#include <avr/boot.h>
#include <avr/cpufunc.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/atomic.h>
#include <util/delay.h>



static void wdt_soft_reset() {
    wdt_enable(WDTO_15MS);
    while (true);
}



uint8_t eeprom EEMEM = 129;



void main(void) __attribute__((OS_main, section(".init9")));


void update_print_page(const update_page_t * const ublock) {
    printf("base: %04x\n", ublock->base_address);

    for (int i = 0; i < SPM_PAGESIZE; i++) {
        printf("%02x ", ublock->data[i]);

        if ((i & 0xF) == 0xF) {
            puts("");
        }
    }
}



static void setup() {
    clock_prescale_set(clock_div_2);                                            // simulate 8MHz device
    power_all_disable();                                                        // disable all components to save power - enable as required

    DDRB = _BV(PB5);

    uart_init_async(0x00);
    puts("SETUP...");
    spi_init();
    // nrf24_init();

#ifdef NRF24_ROLE_COORDINATOR
    nrf24_coordinator_init();
    nrf24_rx_start();
#endif

#ifdef NRF24_ROLE_ROUTER
    nrf24_router_init();
#endif

//    lcd_init();
    time_init();
    sei();

    puts("READY");

//    uint16_t addr = __builtin_return_address(0);
//    printf("0x%04x\n", addr);
}



static void parser(uint8_t c) {
    static ihex_state_t ihex;
    static update_page_t ublock;
    static const uint8_t payload[32] = "Hello World! ABCDEFGHIJKLMNOPQRS";

    if (ihex_parse_async(&ihex, c)) {
        if (ihex.chksum_valid) {
            switch (ihex.type) {
                case 0x00:
                    update_page_add(&ublock, ihex.len, ihex.data, ihex.offset);
                    puts("\tOK");
                    return;

                case 0x06:
                    if (update_write_ota_page(&ublock)) {
                        puts("\tOK");
                    } else {
                        puts("\tERR");
                    }

                    return;

                case 0x16:
                {
                    Update_t *up = (Update_t *)&ihex.data;
                    // puts("writing to EEPROM...");
                    eeprom_update_block(up, (void *)ADDR_EE_UPDATE, sizeof(*up));
                    // puts("DONE");
                    // puts("rebooting...");
                    puts("\tOK");
                    wdt_soft_reset();
                }
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

                case 0x0B:
                    debug_clear_mem();
                    break;

                case 0x0C:
                    debug_dump_signature();
                    break;

                case 0x0D:
                    debug_dump_fuse();
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
}



static void loop() {
    char c;
    uint8_t p;
    uint8_t len;
    static uint8_t tx_len = 1;
    uint8_t rx_payload[32];
    static const uint8_t payload[32] = "Hello World! ABCDEFGHIJKLMNOPQRS";

    if (uart_getchar_async(&c)) {
        parser(c);

        switch (c) {
            case 'r':
                puts("start listening!");
                nrf24_rx_start();
                break;

            case 's':
                nrf24_rx_stop();
                puts("stopped listening!");
                break;

            case 'd':
                nrf24_debug();
                break;

            case 't':
                puts("sending…");
                nrf24_tx(tx_len, payload);
                tx_len++;

                if (tx_len > 32) {
                    tx_len = 1;
                }

                break;

            case 'z':
                BIT_SET(PORTB, _BV(PB1));
                _delay_us(15);
                BIT_CLR(PORTB, _BV(PB1));
                break;

            case 'u':
                BIT_SET(PORTB, _BV(PB1));
                break;

            case 'i':
                BIT_CLR(PORTB, _BV(PB1));
                break;

            case 'q':
                puts("clear interrupts");
                nrf24_clear_interrupts();
                break;

            case 'w':
                puts("clear plos cnt");
                nrf24_clear_plos_cnt();
                break;

            case 'n':
                nrf24_carrier_start();
                break;

            case 'm':
                nrf24_carrier_stop();
                break;

            case 'p':
                nrf24_scan();
                break;

            default:
                break;
        };
    }

    
//    if (nrf24_rx(&p, &len, rx_payload)) {
//        puts("received");
//        printf("pipe: %d\nlen : %d\ndata: ", p, len);
//
//        for (uint8_t i = 0; i < len; i++) {
//            putchar(rx_payload[i]);
//        }
//
//        puts("");
//
//        for (uint8_t i = 0; i < len; i++) {
//            parser(rx_payload[i]);
//        }
//    }

#ifdef NRF24_ROLE_COORDINATOR
    if (nrf24_coordinator_rx(&len, rx_payload)) {
        puts("received");
        printf("pipe: %d\nlen : %d\ndata: ", p, len);

        for (uint8_t i = 0; i < len; i++) {
            putchar(rx_payload[i]);
        }

        puts("");

        for (uint8_t i = 0; i < len; i++) {
            parser(rx_payload[i]);
        }
    }
#endif

#ifdef NRF24_ROLE_ROUTER
    if (nrf24_router_rx(&len, rx_payload)) {
        puts("received");
        printf("pipe: %d\nlen : %d\ndata: ", p, len);

        for (uint8_t i = 0; i < len; i++) {
            putchar(rx_payload[i]);
        }

        puts("");

        for (uint8_t i = 0; i < len; i++) {
            parser(rx_payload[i]);
        }
    }
#endif

    if (nrf24_tx_done()) {
        puts("tx done");
    }

    if (nrf24_tx_fail()) {
        puts("tx fail");
    }

    static uint32_t before = 0;
    uint32_t now = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        now = s_time_ms; // atomic
    }

    // conflicts with SPI
//    if (now >= before + 1000) {
//        before += 1000;
//        BIT_TGL(PORTB, _BV(PB5));
//    }


//    BIT_SET(PORTB, _BV(PB5));
//    _delay_ms(10);
//    BIT_CLR(PORTB, _BV(PB5));
//    _delay_ms(1990);

    // puts("blink");
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
