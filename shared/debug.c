//
//  debug.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 27.01.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "config.h"
#include "debug.h"

#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>




void debug_dump_pgm() {
    // void * memcpy_P(void *dest, const void *src, size_t n);
    uint8_t *addr = (uint8_t *)0x0000;

    do {
        uint8_t chksum = 0x00 - IHEX_DATA_MAX - ((uintptr_t)addr & 0xFF) - ((uintptr_t)addr >> 8);
        printf(":%02X %04" PRIXPTR " 00 ", IHEX_DATA_MAX, (uintptr_t)addr);

        for (uint8_t i = 0; i < IHEX_DATA_MAX; i++) {
            uint8_t d = pgm_read_byte(addr);
            chksum -= d;
            printf("%02X", d);
            addr++;
        }

        printf(" %02X\n", chksum);
    } while (addr < (uint8_t *)FLASHEND);

    puts(":00000001FF");
}



void debug_dump_mem() {
    uint8_t *addr = (uint8_t *)0x000;

    do {
        uint8_t chksum = 0x00 - IHEX_DATA_MAX - ((uintptr_t)addr & 0xFF) - ((uintptr_t)addr >> 8);
        printf(":%02X %04" PRIXPTR " 00 ", IHEX_DATA_MAX, (uintptr_t)addr);

        for (uint8_t i = 0; i < IHEX_DATA_MAX; i++) {
            chksum -= *addr;
            printf("%02X", *addr);
            addr++;
        }

        printf(" %02X\n", chksum);

        if ((addr == (uint8_t *)0x20) || (addr == (uint8_t *)0x60) || (addr == (uint8_t *)RAMSTART)) {
            puts("");
        }
    } while (addr < (uint8_t *)RAMEND);

    puts(":00000001FF");
}



void debug_dump_eep() {
    // void eeprom_read_block(void * __dst, const void * __src, size_t __n);
    uint8_t *addr = (uint8_t *)0x0000;

    do {
        uint8_t chksum = 0x00 - IHEX_DATA_MAX - ((uintptr_t)addr & 0xFF) - ((uintptr_t)addr >> 8);
        printf(":%02X %04" PRIXPTR " 00 ", IHEX_DATA_MAX, (uintptr_t)addr);

        for (uint8_t i = 0; i < IHEX_DATA_MAX; i++) {
            eeprom_busy_wait();
            uint8_t d = eeprom_read_byte(addr);
            chksum -= d;
            printf("%02X", d);
            addr++;
        }

        printf(" %02X\n", chksum);
    } while (addr < (uint8_t *)E2END);

    puts(":00000001FF");
}


