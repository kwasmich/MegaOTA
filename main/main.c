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

#include "ihex/ihex.h"
#include "spi/spi.h"
#include "uart/uart.h"



#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>



uint8_t eeprom EEMEM = 129;



void main(void) __attribute__((OS_main, section (".init9")));



//volatile static uint8_t s_uart_buffer[64];
//volatile static uint8_t s_uart_buffer_fill = 0;
//volatile static bool s_uart_buffer_ready = false;


static void uart_callback(const uint8_t in_BYTE) {
    //printf("0x%02x %c\n", in_BYTE, in_BYTE);
    //printf("%c", in_BYTE);
    //putchar(in_BYTE);
//    s_uart_buffer_ready = in_BYTE == '\r' || in_BYTE == '\n';
//
//    if (!s_uart_buffer_ready) {
//        if (s_uart_buffer_fill < 62) {
//            s_uart_buffer[s_uart_buffer_fill] = in_BYTE;
//            s_uart_buffer_fill++;
//            s_uart_buffer[s_uart_buffer_fill] = 0;
//        } else {
//            puts("Buffer overflow!");
//            s_uart_buffer_fill = 0;
//            s_uart_buffer[0] = 0;
//        }
//    }
}



static void setup() {
    DDRB = _BV(PB5);

    spi_init();
    uart_init_async(&uart_callback, 0x00);
    sei();
}



static void loop() {
    static ihex_state ihex;
    char c;

    if (uart_getchar_async(&c)) {
        if (ihex_parse_async(&ihex, c)) {
            if (ihex.chksum_valid) {
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
                ihex_parse_async(&ihex, ':');
            } else {
                puts("error!");
            }
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
