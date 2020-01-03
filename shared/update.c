//
//  update.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 03.01.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "update.h"

#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
//#include <stdio.h>
#include <string.h>



void update_page_add(update_page_t * const page, const uint8_t len, uint8_t data[static len], const uint16_t in_OFFSET) {
    const uint16_t base = in_OFFSET & ~(SPM_PAGESIZE - 1UL);
    const uint8_t offset = in_OFFSET - base;
//    printf("base: %04x\n", base);
//    printf("offset: %04x\n", offset);

    if (base != page->base_address) {
        memset(page->data, 0xff, sizeof(page->data));
        page->base_address = base;
    }

    memcpy(&page->data[offset], data, len);
}



void update_write_page(const update_page_t * const page) {
    const uint16_t dstAddress = page->base_address;
    const uint8_t sreg = SREG;
    cli();
    boot_spm_busy_wait();
    eeprom_busy_wait();

    for (uint8_t i = 0; i < SPM_PAGESIZE; i += 2) {
        uint16_t val = page->data[i] | (page->data[i + 1] << 8);
        boot_page_fill(dstAddress + i, val);
    }

    boot_page_erase(dstAddress);
    boot_spm_busy_wait();           // Wait until the memory is erased.
    boot_page_write(dstAddress);    // Store buffer in flash page.
    boot_spm_busy_wait();           // Wait until the memory is written.
    boot_rww_enable();
    SREG = sreg;
}
