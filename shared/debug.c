//
//  debug.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 27.01.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "config.h"
#include "debug.h"

#include <avr/boot.h>
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

    puts(":00 0000 01 FF");
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

    puts(":00 0000 01 FF");

    printf("  __data_start: %03X\n", (uintptr_t)&__data_start);
    printf("    __data_end: %03X\n", (uintptr_t)&__data_end);
    printf("   __bss_start: %03X\n", (uintptr_t)&__bss_start);
    printf("     __bss_end: %03X\n", (uintptr_t)&__bss_end);
    printf("      __brkval: %03X\n", (uintptr_t)__brkval);
    printf("__noinit_start: %03X\n", (uintptr_t)&__noinit_start);
    printf("  __noinit_end: %03X\n", (uintptr_t)&__noinit_end);
    printf("  __heap_start: %03X\n", (uintptr_t)&__heap_start);
    printf("            SP: %03X\n", SP);
}



void debug_clear_mem() {
    uint8_t *addr = (uint8_t *)&__bss_end;
    addr++;

    do {
        *addr = 0;
        addr++;
    } while ((uintptr_t)addr < SP);
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

    puts(":00 0000 01 FF");
}



void debug_dump_signature() {
    printf("%02" PRIXPTR ": %02X (Sig 1)\n", (uintptr_t)0, boot_signature_byte_get(0));
    printf("%02" PRIXPTR ": %02X (Sig 2)\n", (uintptr_t)2, boot_signature_byte_get(2));
    printf("%02" PRIXPTR ": %02X (Sig 3)\n", (uintptr_t)4, boot_signature_byte_get(4));
    printf("%02" PRIXPTR ": %02X (RC Oscillator Calibration)\n", (uintptr_t)1, boot_signature_byte_get(1));

    uint8_t *addr = (uint8_t *)0x0000;

    do {
        uint8_t chksum = 0x00 - IHEX_DATA_MAX - ((uintptr_t)addr & 0xFF) - ((uintptr_t)addr >> 8);
        printf(":%02X %04" PRIXPTR " 00 ", IHEX_DATA_MAX, (uintptr_t)addr);

        for (uint8_t i = 0; i < IHEX_DATA_MAX; i++) {
            uint8_t d = boot_signature_byte_get(addr);
            chksum -= d;
            printf("%02X", d);
            addr++;
        }

        printf(" %02X\n", chksum);
    } while (addr < (uint8_t *)0x80);

    puts(":00 0000 01 FF");
}



void debug_dump_fuse() {
    printf("     low: %02X\n", boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS));
    printf("    lock: %02X\n", boot_lock_fuse_bits_get(GET_LOCK_BITS));
    printf("extended: %02X\n", boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS));
    printf("    high: %02X\n", boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS));
}
