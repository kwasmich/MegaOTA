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
#include <util/delay.h>



// this is smaller than the version from <util/crc16.h>
// CRC-16/MODBUS
static uint16_t crc16_update(uint16_t crc, uint8_t const a) {
    crc ^= a;

    for (uint8_t i = 0; i < 8; i++) {
        if (crc bitand 1) {
            crc = (crc >> 1) xor 0xA001;
        } else {
            crc = (crc >> 1);
        }
    }

    return crc;
}



uint8_t eeprom EEMEM = 129;



void main(void) __attribute__((OS_main, section(".init9")));



static void setup() {
    clock_prescale_set(clock_div_2);                                            // simulate 8MHz device
    power_all_disable();                                                        // disable all components to save power - enable as required

    DDRB = _BV(PB5);

//    spi_init();
//    nrf24_init();
    uart_init_async(0x00);
//    lcd_init();
    time_init();

    puts("READY");
}



static bool isSignatureMatching(const Update_t * const up) {
    uint32_t sig = ((uint32_t)boot_signature_byte_get(0) << 16) bitor ((uint32_t)boot_signature_byte_get(2) << 8) bitor boot_signature_byte_get(4);
    uint32_t usig = ((uint32_t)up->signature[0] << 16) bitor ((uint32_t)up->signature[1] << 8) bitor up->signature[2];
    return sig == usig;
}



static bool areFusesMatching(const Update_t * const up) {
    uint16_t fuse = (boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS) << 8) bitor boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
    uint16_t ufuse = (up->hfuse << 8) bitor up->lfuse;
    // uint8_t efuse = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
    // uint8_t lock  = boot_lock_fuse_bits_get(GET_LOCK_BITS);
    return fuse == ufuse;
}



static uint8_t read_byte(uint16_t const address) {
    uint8_t val;
#ifdef EXTERNAL_MEMORY
#   if EXTERNAL_MEMORY == EXTERNAL_MEMORY_I2C
    val = i2c_eeprom_read_byte(address);
#   elif EXTERNAL_MEMORY == EXTERNAL_MEMORY_SPI
    val = spi_eeprom_read_byte(address);
#   else
#       error not implemented
#   endif
#else
    val = pgm_read_byte(address);
#endif
    return val;
}



static uint16_t read_word(uint16_t const address) {
    uint16_t val;
#ifdef EXTERNAL_MEMORY
#   if EXTERNAL_MEMORY == EXTERNAL_MEMORY_I2C
    val  = i2c_eeprom_read_byte(address) << 8;
    val |= i2c_eeprom_read_next_byte();
#   elif EXTERNAL_MEMORY == EXTERNAL_MEMORY_SPI
    spi_eeprom_read_start(address);
    val  = spi_eeprom_read_next() << 8;
    val |= spi_eeprom_read_next();
#   else
#       error not implemented
#   endif
#else
    val = pgm_read_word(address);
#endif
    return val;
}



static bool isCRCMatching(uint16_t const addr, uint8_t const numPages, uint16_t const dst_CRC) {
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < numPages * SPM_PAGESIZE; i++) {
        uint8_t val = read_byte(addr + i);
        crc = crc16_update(crc, val);
    }

    return crc == dst_CRC;
}



static void update(uint16_t const dstAddr, uint16_t const srcAddr, uint8_t numPages) {
//    eeprom_busy_wait();
    puts("Start update...");

    while (numPages) {
        numPages--;
        uint16_t dstAddress = dstAddr + (SPM_PAGESIZE * numPages);
        uint16_t srcAddress = srcAddr + (SPM_PAGESIZE * numPages);
        printf("%04x -> %04x\n", srcAddress, dstAddress);

//        boot_page_erase(dstAddress);
//        boot_spm_busy_wait();      // Wait until the memory is erased.

        for (uint8_t i = 0; i < SPM_PAGESIZE; i += 2) {
            printf("%04x\n", srcAddress + i);
//            uint16_t val = read_word(srcAddress + i);
//            boot_page_fill(dstAddress + i, val);
        }

//        boot_page_write(dstAddress);     // Store buffer in flash page.
//        boot_spm_busy_wait();       // Wait until the memory is written.
    }

//    boot_rww_enable();
    puts("... update done!");
}



static void parser(uint8_t c) {
    static ihex_state_t ihex;
    static update_page_t ublock;
    static const uint8_t payload[32] = "Hello World! ABCDEFGHIJKLMNOPQRS";

    if (ihex_parse_async(&ihex, c)) {
        if (ihex.chksum_valid) {
            puts("\tOK");
            switch (ihex.type) {
                case 0x00:
                    update_page_add(&ublock, ihex.len, ihex.data, ihex.offset);

//                    printf("base: %04x\n", ublock.base_address);
//
//                    for (int i = 0; i < SPM_PAGESIZE; i++) {
//                        printf("%02x ", ublock.data[i]);
//
//                        if ((i & 0xF) == 0xF) {
//                            puts("");
//                        }
//                    }

                    return;
//                    break;

                case 0x06:
//                    puts("writing to PROGMEM...");
                    update_write_page(&ublock);
//                    puts("DONE");
                    return;
//                    break;

                case 0x16:
                {
                    Update_t *up = (Update_t *)&ihex.data;

                    puts("writing to EEPROM...");
                    eeprom_update_block(up, (void *)ADDR_EE_UPDATE, sizeof(*up));
                    puts("DONE");
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

                case 0x0E:
                {
                    Update_t up = { .main_page_count = 0 };
                    up.signature[0] = boot_signature_byte_get(0);
                    up.signature[1] = boot_signature_byte_get(2);
                    up.signature[2] = boot_signature_byte_get(4);
                    up.main_page_count = 1;
                    up.write_page_count = 0;
                    up.lfuse = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
                    up.hfuse = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
                    up.efuse = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
                    up.lock = boot_lock_fuse_bits_get(GET_LOCK_BITS);

                    uint16_t crc = 0xFFFF;
                    const uint16_t numPages = 1;
                    const uint8_t *addr = 0x3800;

                    for (uint16_t i = 0; i < numPages * SPM_PAGESIZE; i++) {
                        uint8_t val = pgm_read_byte(addr + i);
                        crc = crc16_update(crc, val);
                    }

                    up.main_crc = crc;
                    up.write_crc = 0xFFFF;
                    up.boot_crc = 0xFFFF;

                    printf("main_crc : 0x%04x\n", up.main_crc);
                    printf("write_crc: 0x%04x\n", up.write_crc);
                    printf("boot_crc : 0x%04x\n", up.boot_crc);
                    printf("main_page_count : %d\n", up.main_page_count);
                    printf("write_page_count: %d\n", up.write_page_count);
                    printf("boot_page_count : %d\n", up.boot_page_count);
                    printf("l: 0x%02x, h: 0x%02x, e: 0x%02x, lock: 0x%02x\n", up.lfuse, up.hfuse, up.efuse, up.lock);
                    printf("signature: %02x %02x %02x\n", up.signature[0], up.signature[1], up.signature[2]);
                    eeprom_update_block(&up, (void *)ADDR_EE_UPDATE, sizeof(up));
                }
                    
                    break;

                case 0x0F:
                {

#define IN_RANGE(X, Y, Z) (((X) <= (Y)) and ((Y) <= (Z)))
#define NUM_WRITE_OTA_PAGES NUM_WRITE_PAGES
#define NUM_BOOT_OTA_PAGES  NUM_BOOT_PAGES
#define NUM_MAIN_PAGES      ((NUM_PAGES - NUM_WRITE_PAGES - NUM_BOOT_PAGES - NUM_WRITE_OTA_PAGES - NUM_BOOT_OTA_PAGES) / 2)
#define NUM_MAIN_OTA_PAGES  NUM_MAIN_PAGES

#define ADDR_MAIN      0x0000
#define ADDR_MAIN_OTA  (ADDR_MAIN + NUM_MAIN_PAGES * SPM_PAGESIZE)
#define ADDR_WRITE_OTA (ADDR_MAIN_OTA + NUM_MAIN_OTA_PAGES * SPM_PAGESIZE)
#define ADDR_BOOT_OTA  (ADDR_WRITE_OTA + NUM_WRITE_OTA_PAGES * SPM_PAGESIZE)
#define ADDR_BOOT      (ADDR_BOOT_OTA + NUM_BOOT_OTA_PAGES * SPM_PAGESIZE)
#define ADDR_WRITE     (ADDR_BOOT + NUM_BOOT_PAGES * SPM_PAGESIZE)

                    Update_t up;
                    eeprom_read_block(&up, (void *)ADDR_EE_UPDATE, sizeof(up));

                    printf("main_crc : 0x%04x\n", up.main_crc);
                    printf("write_crc: 0x%04x\n", up.write_crc);
                    printf("boot_crc : 0x%04x\n", up.boot_crc);
                    printf("main_page_count : %d\n", up.main_page_count);
                    printf("write_page_count: %d\n", up.write_page_count);
                    printf("boot_page_count : %d\n", up.boot_page_count);
                    printf("l: 0x%02x, h: 0x%02x, e: 0x%02x, lock: 0x%02x\n", up.lfuse, up.hfuse, up.efuse, up.lock);
                    printf("signature: %02x %02x %02x\n", up.signature[0], up.signature[1], up.signature[2]);

                    printf("A: %d\n", IN_RANGE(1, up.main_page_count, NUM_MAIN_PAGES));
                    printf("B: %d\n", IN_RANGE(1, up.write_page_count, NUM_WRITE_PAGES));
                    printf("C: %d\n", isSignatureMatching(&up));
                    printf("D: %d\n", areFusesMatching(&up));
                    printf("E: %d\n", isCRCMatching(ADDR_MAIN_OTA, up.main_page_count, up.main_crc));
                    printf("F: %d\n", isCRCMatching(ADDR_WRITE_OTA, up.write_page_count, up.write_crc));

                    uint8_t main_page_count = up.main_page_count;
                    uint8_t write_page_count = up.write_page_count;

                    if (IN_RANGE(1, main_page_count, NUM_MAIN_PAGES) or IN_RANGE(1, write_page_count, NUM_WRITE_PAGES)) {
                        eeprom_update_byte((void *)ADDR_EE_UPDATE + offsetof(Update_t, main_page_count), 0);
                        eeprom_update_byte((void *)ADDR_EE_UPDATE + offsetof(Update_t, write_page_count), 0);
                        eeprom_busy_wait();

                        if (!isSignatureMatching(&up)) {
                            puts("Signature mismatch!");
                            goto theend;
                        }

                        if (!areFusesMatching(&up)) {
                            puts("Fuses mismatch!");
                            goto theend;
                        }

#ifdef EXTERNAL_MEMORY
#   if EXTERNAL_MEMORY == EXTERNAL_MEMORY_I2C
                        i2c_init();
#   elif EXTERNAL_MEMORY == EXTERNAL_MEMORY_SPI
                        spi_init();
                        spi_eeprom_init();
#   else
#       error not implemented
#   endif
#endif

                        if (!isCRCMatching(ADDR_MAIN_OTA, up.main_page_count, up.main_crc)) {
                            puts("CRC mismatch on main!");
                            goto theend;
                        }

                        if (!isCRCMatching(ADDR_WRITE_OTA, up.write_page_count, up.write_crc)) {
                            puts("CRC mismatch on write!");
                            goto theend;
                        }

                        update(ADDR_MAIN, ADDR_MAIN_OTA, up.main_page_count);
                        update(ADDR_WRITE, ADDR_WRITE_OTA, up.write_page_count);
                        puts("Update successful!");
//                        wdt_soft_reset();
                    }

                theend:
                    puts("The End!");

                }
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


    static uint32_t before = 0;

    cli();
    uint32_t now = s_time_ms; // atomic
    sei();

    if (now >= before + 1000) {
        before += 1000;
        BIT_TGL(PORTB, _BV(PB5));
    }


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
