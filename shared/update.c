//
//  update.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 03.01.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "update.h"

#include <avr/boot.h>
#include <avr/interrupt.h>
#include <string.h>



bool isValidBaseAddress(uint16_t const in_BASE_ADDRESS) {
    switch (in_BASE_ADDRESS) {
        case ADDR_MAIN_OTA_START ... ADDR_MAIN_OTA_END - 2:
        case ADDR_WRITE_OTA_START ... ADDR_WRITE_OTA_END - 2:
        case ADDR_BOOT_OTA_START ... ADDR_BOOT_OTA_END - 2:
            return true;
        default:
            return false;
    }
}



void update_page_add(update_page_t * const page, const uint8_t len, uint8_t data[static len], const uint16_t in_OFFSET) {
    const uint16_t base = in_OFFSET & ~(SPM_PAGESIZE - 1UL);
    const uint8_t offset = in_OFFSET - base;

    if (base != page->base_address) {
        memset(page->data, 0xff, sizeof(page->data));
        page->base_address = base;
    }

    memcpy(&page->data[offset], data, len);
}



bool update_write_ota_page(const update_page_t * const page) {
#warning handle external storage

    if (!isValidBaseAddress(page->base_address)) {
        return false;
    }

    const uint16_t dstAddress = page->base_address;
    const uint8_t sreg = SREG;
    cli();
    boot_spm_busy_wait();
    eeprom_busy_wait();

    for (uint8_t i = 0; i < SPM_PAGESIZE / 2; i++) {
        boot_page_fill(dstAddress + (i * 2), page->word[i]);
    }

    boot_page_erase(dstAddress);
    boot_spm_busy_wait();           // Wait until the memory is erased.
    boot_page_write(dstAddress);    // Store buffer in flash page.
    boot_spm_busy_wait();           // Wait until the memory is written.
    boot_rww_enable();
    SREG = sreg;
    return true;
}


//void update_write_page(const update_page_t * const page) {
//#warning handle external storage
//
//    const uint16_t dstAddress = page->base_address;
//    boot_spm_busy_wait();
//    eeprom_busy_wait();
//
//    for (uint8_t i = 0; i < SPM_PAGESIZE / 2; i++) {
//        boot_page_fill(dstAddress + (i * 2), page->word[i]);
//    }
//
//    boot_page_erase(dstAddress);
//    boot_spm_busy_wait();           // Wait until the memory is erased.
//    boot_page_write(dstAddress);    // Store buffer in flash page.
//    boot_spm_busy_wait();           // Wait until the memory is written.
//    boot_rww_enable();
//}
//
//
//
//bool update_write_ota_page(const update_page_t * const page) {
//#warning handle external storage
//
//    if (!isValidBaseAddress(page->base_address)) {
//        return false;
//    }
//
//    const uint8_t sreg = SREG;
//    cli();
//    update_write_page(page);
//    SREG = sreg;
//    return true;
//}
