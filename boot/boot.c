//
//  boot.c
//  Blink
//
//  Created by Michael Kwasnicki on 2018-02-09.
//  Copyright © 2018 Michael Kwasnicki. All rights reserved.
//


#include "config.h"
#include "macros.h"
#include "update.h"
void update_write_page(const update_page_t * const update_block);               // override section attribute
//#include "crypto/crc.h"

#ifdef EXTERNAL_MEMORY
#   if EXTERNAL_MEMORY == EXTERNAL_MEMORY_I2C
#       include "i2c/at24cxx.h"
#   elif EXTERNAL_MEMORY == EXTERNAL_MEMORY_SPI
#       include "spi/spi.h"
#       include "spi/sst25vfxx.h"
#   else
#       error not implemented
#   endif
#endif

#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/lock.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>


#pragma message MACRO_VALUE(FLASHEND)
#pragma message MACRO_VALUE(SPM_PAGESIZE)

//#define IN_RANGE(X, Y, Z) (((X) <= (Y)) and ((Y) <= (Z)))



FUSES = {
    .low =      LFUSE,
    .high =     HFUSE,
    .extended = EFUSE
};

LOCKBITS = LOCKFUSE;



//char myEepromString[] EEMEM = "Hello World!";




//#ifdef EXTERNAL_MEMORY
//#   define NUM_WRITE_OTA_PAGES 0
//#   define NUM_BOOT_OTA_PAGES  0
//#   define NUM_MAIN_PAGES      (NUM_PAGES - NUM_WRITE_PAGES - NUM_BOOT_PAGES - NUM_WRITE_OTA_PAGES - NUM_BOOT_OTA_PAGES)
//#   define NUM_MAIN_OTA_PAGES  0
//
//#   define ADDR_MAIN      0x0000
//#   define ADDR_WRITE     (ADDR_MAIN + NUM_MAIN_PAGES * SPM_PAGESIZE)
//#   define ADDR_BOOT      (ADDR_WRITE + NUM_WRITE_PAGES * SPM_PAGESIZE)
//#   define ADDR_MAIN_OTA  ADDR_MAIN
//#   define ADDR_WRITE_OTA ADDR_WRITE
//#   define ADDR_BOOT_OTA  ADDR_BOOT
//#else
//#   define NUM_WRITE_OTA_PAGES NUM_WRITE_PAGES
//#   define NUM_BOOT_OTA_PAGES  NUM_BOOT_PAGES
//#   define NUM_MAIN_PAGES      ((NUM_PAGES - NUM_WRITE_PAGES - NUM_BOOT_PAGES - NUM_WRITE_OTA_PAGES - NUM_BOOT_OTA_PAGES) / 2)
//#   define NUM_MAIN_OTA_PAGES  NUM_MAIN_PAGES
//
//#   define ADDR_MAIN      0x0000
//#   define ADDR_MAIN_OTA  (ADDR_MAIN + NUM_MAIN_PAGES * SPM_PAGESIZE)
//#   define ADDR_WRITE_OTA (ADDR_MAIN_OTA + NUM_MAIN_OTA_PAGES * SPM_PAGESIZE)
//#   define ADDR_BOOT_OTA  (ADDR_WRITE_OTA + NUM_WRITE_OTA_PAGES * SPM_PAGESIZE)
//#   define ADDR_BOOT      (ADDR_BOOT_OTA + NUM_BOOT_OTA_PAGES * SPM_PAGESIZE)
//#   define ADDR_WRITE     (ADDR_BOOT + NUM_BOOT_PAGES * SPM_PAGESIZE)
//#endif


#if ! defined ADDR_MAIN_START
#   error ADDR_MAIN_START is undefined
#elif ! defined ADDR_MAIN_END
#   error ADDR_MAIN_END is undefined
#elif ! defined ADDR_MAIN_OTA_START
#   error ADDR_MAIN_OTA_START is undefined
#elif ! defined ADDR_MAIN_OTA_END
#   error ADDR_MAIN_OTA_END is undefined
#elif ! defined ADDR_WRITE_START
#   error ADDR_WRITE_START is undefined
#elif ! defined ADDR_WRITE_END
#   error ADDR_WRITE_END is undefined
#elif ! defined ADDR_WRITE_OTA_START
#   error ADDR_WRITE_OTA_START is undefined
#elif ! defined ADDR_WRITE_OTA_END
#   error ADDR_WRITE_OTA_END is undefined
#elif ! defined ADDR_BOOT_START
#   error BOOTWRITE_START is undefined
#elif ! defined ADDR_BOOT_END
#   error ADDR_BOOT_END is undefined
#elif ! defined ADDR_BOOT_OTA_START
#   error ADDR_BOOT_OTA_START is undefined
#elif ! defined ADDR_BOOT_OTA_END
#   error ADDR_BOOT_OTA_END is undefined
#endif

#pragma message MACRO_VALUE(ADDR_MAIN_START)
#pragma message MACRO_VALUE(ADDR_MAIN_END)
#pragma message MACRO_VALUE(ADDR_MAIN_OTA_START)
#pragma message MACRO_VALUE(ADDR_MAIN_OTA_END)
#pragma message MACRO_VALUE(ADDR_WRITE_START)
#pragma message MACRO_VALUE(ADDR_WRITE_END)
#pragma message MACRO_VALUE(ADDR_WRITE_OTA_START)
#pragma message MACRO_VALUE(ADDR_WRITE_OTA_END)
#pragma message MACRO_VALUE(ADDR_BOOT_START)
#pragma message MACRO_VALUE(ADDR_BOOT_END)
#pragma message MACRO_VALUE(ADDR_BOOT_OTA_START)
#pragma message MACRO_VALUE(ADDR_BOOT_OTA_END)

#define NUM_MAIN_PAGES ((ADDR_MAIN_END - ADDR_MAIN_START) / SPM_PAGESIZE)
#define NUM_WRITE_PAGES ((ADDR_WRITE_END - ADDR_WRITE_START) / SPM_PAGESIZE)



// naked: no entry and exit code (no stack)
void wdt_init(void) __attribute__((naked, section(".init3")));
void main(void) __attribute__((OS_main, section(".init9")));



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



static void update(uint16_t const dstAddr, uint16_t const srcAddr, uint8_t numPages) {
    update_page_t page;
    const uint8_t sreg = SREG;
    cli();

    while (numPages) {
        numPages--;
        page.base_address = dstAddr + numPages * SPM_PAGESIZE;

        for (uint8_t i = 0; i < SPM_PAGESIZE; i++) {
            page.data[i] = read_byte(srcAddr + (uint16_t)numPages * SPM_PAGESIZE + i);
        }

        // for (uint8_t i = 0; i < SPM_PAGESIZE / 2; i++) {
        //    page.word[i] = read_word(srcAddr + (uint16_t)numPages * SPM_PAGESIZE + i * 2);
        // }

        // memcpy_P(page.data, srcAddr + (uint16_t)numPages * SPM_PAGESIZE, SPM_PAGESIZE);
        update_write_page(&page);
    }

    // boot_spm_busy_wait();
    // eeprom_busy_wait();
    //
    // while (numPages) {
    //     numPages--;
    //     uint16_t dstAddress = dstAddr + (SPM_PAGESIZE * numPages);
    //     uint16_t srcAddress = srcAddr + (SPM_PAGESIZE * numPages);
    //
    //     for (uint8_t i = 0; i < SPM_PAGESIZE; i += 2) {
    //         uint16_t val = read_word(srcAddress + i);
    //         boot_page_fill(dstAddress + i, val);
    //     }
    //
    //     boot_page_erase(dstAddress);
    //     boot_spm_busy_wait();                                                   // Wait until the memory is erased.
    //     boot_page_write(dstAddress);                                            // Store buffer in flash page.
    //     boot_spm_busy_wait();                                                   // Wait until the memory is written.
    //     boot_rww_enable();                                                      // erase and write disable RWW section, but we need it for reading
    // }

    SREG = sreg;
}



static void wdt_soft_reset() {
    wdt_enable(WDTO_15MS);
    while (true);
}



static bool isSignatureMatching(const Update_t * const up) {
    if (boot_signature_byte_get(0) != up->signature[0]) return false;
    if (boot_signature_byte_get(2) != up->signature[1]) return false;
    if (boot_signature_byte_get(4) != up->signature[2]) return false;
    return true;
}



static bool areFusesMatching(const Update_t * const up) {
    if (boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS) != up->lfuse) return false;
    if (boot_lock_fuse_bits_get(GET_LOCK_BITS) != up->lock) return false;
    if (boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS) != up->efuse) return false;
    if (boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS) != up->hfuse) return false;
    return true;
}



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



static bool isCRCMatching(uint16_t const addr, uint8_t const numPages, uint16_t const dst_CRC) {
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < numPages * SPM_PAGESIZE; i++) {
        uint8_t val = read_byte(addr + i);
        crc = crc16_update(crc, val);
    }

    return crc == dst_CRC;
}



void wdt_init() {
    // wdt_reset(); // not doing this should be safe as it will be called in wdt_disable()
    MCUSR = 0x00;
    wdt_disable();
}



void main() {
    uint8_t err;
    DDRB = _BV(PB5);

    Update_t up;
    eeprom_read_block(&up, (void *)ADDR_EE_UPDATE, sizeof(up));

    if (up.prog_update) {                                                       // there seems to be an update available
        eeprom_update_byte((uint8_t *)ADDR_EE_UPDATE + offsetof(Update_t, prog_update), 0);
        eeprom_busy_wait();

        if (!isSignatureMatching(&up)) {                                        // signature mismatch
            err = 2;
            goto error;
        }

        if (!areFusesMatching(&up)) {                                           // fuse mismatch
            err = 4;
            goto error;
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

        if (!isCRCMatching(ADDR_MAIN_OTA_START, NUM_MAIN_PAGES, up.main_crc)) {   // crc mismatch
            err = 6;
            goto error;
        }

        if (!isCRCMatching(ADDR_WRITE_OTA_START, NUM_WRITE_PAGES, up.write_crc)) {// crc mismatch
            err = 8;
            goto error;
        }

        // update
        update(ADDR_MAIN_START, ADDR_MAIN_OTA_START, NUM_MAIN_PAGES);
        update(ADDR_WRITE_START, ADDR_WRITE_OTA_START, NUM_WRITE_PAGES);
        wdt_soft_reset();
    }

    BIT_TGL(PORTB, _BV(PB5));
    _delay_ms(500);

    for (uint8_t i = 0; i < 20; i++) {
        BIT_TGL(PORTB, _BV(PB5));
        _delay_ms(50);
    }

    _delay_ms(450);
    BIT_TGL(PORTB, _BV(PB5));
    _delay_ms(500);

start:
    asm("jmp 0x0000");

error:
    while (true) {
        for (uint8_t i = 0; i < err; i++) {
            BIT_TGL(PORTB, _BV(PB5));
            _delay_ms(500);
        }
        _delay_ms(5000);
    }
    return;
}

