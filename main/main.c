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
#include "lcd/lcd.h"
#include "crypto/entropy.h"



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
#include <util/delay.h>









uint8_t eeprom EEMEM = 129;



void main(void) __attribute__((OS_main, section(".init9")));



static void setup() {
    clock_prescale_set(clock_div_2);                                            // simulate 8MHz device
    power_all_disable();                                                        // disable all components to save power - enable as required

    DDRB = _BV(PB5);

    spi_init();
    nrf24_init();
    uart_init_async(0x00);
    lcd_init();
    sei();
    puts("READY");
    const char hello[] = "Hello, World!\n";
    fwrite(hello, sizeof(hello[0]), strlen(hello), stdout);
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

                case 0x0B:
                    debug_clear_mem();
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
            puts("sending…");
            nrf24_tx(tx_len, payload);
            tx_len++;

            if (tx_len > 32) {
                tx_len = 1;
            }
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
        if (c == 'q') {
            puts("clear interrupts");
            nrf24_clear_interrupts();
        }
        if (c == 'w') {
            puts("clear plos cnt");
            nrf24_clear_plos_cnt();
        }
        if (c == 'n') {
            nrf24_carrier_start();
        }
        if (c == 'm') {
            nrf24_carrier_stop();
        }
    }

    if (nrf24_rx(&p, &len, rx_payload)) {
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



#define SIGNAL_NONE     0
#define SIGNAL_STRENGTH 1
#define SIGNAL_QUALITY  2
#define SIGNAL_RX       1
#define SIGNAL_TX       2

static uint8_t signal_mode = SIGNAL_NONE;
static uint8_t signal_node = SIGNAL_NONE;
static uint8_t payload[32] = "Hello, World!___________________";
static uint16_t tx_count = 0;
static uint16_t tx_fail_count = 0;
static uint16_t tx_win_count = 0;
static uint16_t rx_count = 0;



static void lap() {
    static uint16_t t = 0;
    static uint16_t r = 0;
    static uint16_t f = 0;

    const uint8_t rf = (rx_count - r) / 16;

    lcd_clear_display();
    lcd_goto_line(0);

    for (uint8_t i = 0; i < rf; i++) {
        putchar(0xFF);
    }

    lcd_goto_line(1);

    switch (signal_mode) {
        case SIGNAL_STRENGTH:
            fputs("Signal Srength", stdout);
            break;

        case SIGNAL_QUALITY:
            fputs("Signal Quality", stdout);
            break;
    }

    t = tx_count;
    r = rx_count;
    f = tx_fail_count;

    //printf("t:%5u  w:%5u\nf:%5u  r:%5u\n", tx_count, tx_win_count, tx_fail_count, rx_count);
}



static void tx() {
    if ((tx_count & 0xFF) == 0) {
        lap();
    }

    if (tx_count != UINT16_MAX) {
//        fputs("TX ... ", stdout);
//        entropy(4, &payload[28]);
        nrf24_tx(32, payload);
        tx_count++;
    } else {
        printf("t:%5u  w:%5u\nf:%5u  r:%5u\n", tx_count, tx_win_count, tx_fail_count, rx_count);
    }
}



static void setup_ptx_quality() {
    tx();
}



static void loop_ptx_quality() {
    uint8_t p;
    uint8_t len;
    uint8_t rx_payload[32];

    if (nrf24_tx_fail()) {
        tx_fail_count++;
        nrf24_flush_tx();
        tx();
    }

    if (nrf24_tx_done()) {
        tx_win_count++;
        tx();
    }

    if (nrf24_rx(&p, &len, rx_payload)) {
        rx_count++;
    }
}



static void setup_prx_quality() {
    memset(payload, 0, sizeof(payload));
    nrf24_enqueue_ack_payload(0, sizeof(payload), payload);
    nrf24_rx_start();
}



static void loop_prx_quality() {
    uint8_t p;
    uint8_t len;
    uint8_t rx_payload[32];

    if (nrf24_rx(&p, &len, rx_payload)) {
        nrf24_enqueue_ack_payload(0, sizeof(rx_payload), rx_payload);
    }
}



static void loop_prx_strength() {
    _delay_ms(1);
    uint8_t u8 = nrf24_io_command_1(R_REGISTER xor RPD, 0xFF);

    if ((tx_count & 0xFF) == 0) {
        lap();
    }

    if (tx_count != UINT16_MAX) {
        tx_count++;
        rx_count += u8;
    } else {
        printf("r:%5u  w:%5u\n\n", tx_count, rx_count);
        exit(0);
    }
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

    BIT_SET(PORTD, _BV3(PD5, PD6, PD7));
    BIT_CLR(DDRD, _BV3(PD5, PD6, PD7));
    _NOP();
    uint8_t pind = PIND;
    if ((pind & _BV(PD7)) == 0) {
        signal_mode = SIGNAL_QUALITY;
        signal_node = (pind & _BV(PD5)) ? SIGNAL_RX : SIGNAL_TX;
    } else if ((pind & _BV(PD6)) == 0) {
        signal_mode = SIGNAL_STRENGTH;
        signal_node = (pind & _BV(PD5)) ? SIGNAL_TX : SIGNAL_RX;
    }

    if (signal_mode == SIGNAL_STRENGTH) {
        lcd_clear_display();
        puts("Signal Strenght");

        if (signal_node == SIGNAL_TX) {
            puts("TX");
            _delay_ms(3000);
            nrf24_carrier_start();
        } else if (signal_node == SIGNAL_RX) {
            puts("RX");
            _delay_ms(3000);
            nrf24_rx_start();

            do {
                loop_prx_strength();
            } while (true);
        }
    } else if (signal_mode == SIGNAL_QUALITY) {
        lcd_clear_display();
        puts("Signal Quality");

        if (signal_node == SIGNAL_TX) {
            puts("TX");
            _delay_ms(3000);
            setup_ptx_quality();

            do {
                loop_ptx_quality();
            } while (true);
        } else if (signal_node == SIGNAL_RX) {
            puts("RX");
            _delay_ms(3000);
            setup_prx_quality();

            do {
                loop_prx_quality();
            } while (true);
        }
    } else {
        do {
            loop();
        } while (true);
    }

//    write();
//    wdt_enable(WDTO_15MS);
//    while (true);
}
