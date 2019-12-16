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
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>


#pragma message MACRO_VALUE(NUM_PAGES)
#pragma message MACRO_VALUE(FLASHEND)
#pragma message MACRO_VALUE(SPM_PAGESIZE)
#pragma message MACRO_VALUE(NUM_PAGES2)

#define IN_RANGE(X, Y, Z) (((X) <= (Y)) and ((Y) <= (Z)))


//#pragma mark -- fuse
//
//FUSES = {
//    .low =      (0xff),
//    .high =     (FUSE_BOOTRST & FUSE_BOOTSZ0 & FUSE_EESAVE & FUSE_SPIEN),
//    .extended = (FUSE_BODLEVEL0)
//};
//
//LOCKBITS = (0x3f);
//
//
//
//#pragma mark -- eeprom
//
//char myEepromString[] EEMEM = "Hello World!";





#ifdef EXTERNAL_MEMORY
#   define NUM_WRITE_OTA_PAGES 0
#   define NUM_BOOT_OTA_PAGES  0
#   define NUM_MAIN_PAGES      (NUM_PAGES - NUM_WRITE_PAGES - NUM_BOOT_PAGES - NUM_WRITE_OTA_PAGES - NUM_BOOT_OTA_PAGES)
#   define NUM_MAIN_OTA_PAGES  0

#   define ADDR_MAIN      0x0000
#   define ADDR_WRITE     (ADDR_MAIN + NUM_MAIN_PAGES * SPM_PAGESIZE)
#   define ADDR_BOOT      (ADDR_WRITE + NUM_WRITE_PAGES * SPM_PAGESIZE)
#   define ADDR_MAIN_OTA  ADDR_MAIN
#   define ADDR_WRITE_OTA ADDR_WRITE
#   define ADDR_BOOT_OTA  ADDR_BOOT
#else
#   define NUM_WRITE_OTA_PAGES NUM_WRITE_PAGES
#   define NUM_BOOT_OTA_PAGES  NUM_BOOT_PAGES
#   define NUM_MAIN_PAGES      ((NUM_PAGES - NUM_WRITE_PAGES - NUM_BOOT_PAGES - NUM_WRITE_OTA_PAGES - NUM_BOOT_OTA_PAGES) / 2)
#   define NUM_MAIN_OTA_PAGES  NUM_MAIN_PAGES

#   define ADDR_MAIN      0x0000
#   define ADDR_MAIN_OTA  (ADDR_MAIN + NUM_MAIN_PAGES * SPM_PAGESIZE)
#   define ADDR_WRITE_OTA (ADDR_MAIN_OTA + NUM_MAIN_OTA_PAGES * SPM_PAGESIZE)
#   define ADDR_BOOT_OTA  (ADDR_WRITE_OTA + NUM_WRITE_OTA_PAGES * SPM_PAGESIZE)
#   define ADDR_WRITE     (ADDR_BOOT_OTA + NUM_BOOT_OTA_PAGES * SPM_PAGESIZE)
#   define ADDR_BOOT      (ADDR_WRITE + NUM_WRITE_PAGES * SPM_PAGESIZE)
#endif

#define ADDR_EE_UPDATE (E2END + 1 - sizeof(Update_t))



static uint8_t read_byte(uint16_t address) {
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



static uint16_t read_word(uint16_t address) {
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



// naked: no entry and exit code (no stack)
void wdt_init(void) __attribute__((naked, section(".init3")));
void main(void) __attribute__((OS_main, section (".init9")));


// bootloader overflow guard
const uint8_t __attribute__((section(".boot_guard"))) boot_version = 0x42;



static void update(uint16_t dstAddr, uint16_t srcAddr, uint8_t numPages) {
    eeprom_busy_wait();

    do {
        numPages--;
        uint16_t dstAddress = dstAddr + (SPM_PAGESIZE * numPages);
        uint16_t srcAddress = srcAddr + (SPM_PAGESIZE * numPages);
        boot_page_erase(dstAddress);
        boot_spm_busy_wait();      // Wait until the memory is erased.

        for (uint8_t i = 0; i < SPM_PAGESIZE; i += 2) {
            uint16_t val = read_word(srcAddress + i);
            boot_page_fill(dstAddress + i, val);
        }

        boot_page_write(dstAddress);     // Store buffer in flash page.
        boot_spm_busy_wait();       // Wait until the memory is written.
    } while (numPages);

    boot_rww_enable();
}



void wdt_init(void) {
    // wdt_reset(); // not doing this should be safe as it will be called in wdt_disable()
    MCUSR = 0x00;
    wdt_disable();
}



/*
#define wdt_soft_reset() do { \
                         wdt_enable(WDTO_15MS); \
                         while (true); \
                     } while(0)
*/

static void wdt_soft_reset() {
    wdt_enable(WDTO_15MS);
    while (true);
}



// this is smaller than the version from <util/crc16.h>
static uint16_t crc16_update(uint16_t crc, uint8_t a) {
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



static bool isCRCMatching(uint16_t addr, uint8_t numPages, uint16_t dst_CRC) {
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < numPages * SPM_PAGESIZE; i++) {
        uint8_t val = read_byte(addr + i);
        crc = crc16_update(crc, val);
    }

    return crc == dst_CRC;
}



void main(void) {
    // set all ports to output and low to reduce power consumption
    // DDRB = 0xFF;
    // DDRC = 0x7F;
    // DDRD = 0xFF;
    // PORTB = 0x00;
    // PORTC = 0x00;
    // PORTD = 0x00;
/*
    Update_t up;
    eeprom_read_block(&up, (void *)ADDR_EE_UPDATE, sizeof(Update_t));

    if (IN_RANGE(1, up.main_page_count, NUM_MAIN_PAGES) and IN_RANGE(1, up.write_page_count, NUM_WRITE_PAGES)) {
        // there seems to be an update available

        // 54
        if (!isSignatureMatching(&up)) {
            // signature mismatch
            goto start;
        }

        // 38
        if (!areFusesMatching(&up)) {
            // fuse mismatch
            goto start;
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

        // 154
        if (!isCRCMatching(ADDR_MAIN_OTA, up.main_page_count, up.main_crc)) {
            // crc mismatch
            goto start;
        }

        // 82
        if (!isCRCMatching(ADDR_WRITE_OTA, up.write_page_count, up.write_crc)) {
            // crc mismatch
            goto start;
        }

        // update
        // 36
        update(ADDR_MAIN, ADDR_MAIN_OTA, up.main_page_count);

        // 46
        update(ADDR_WRITE, ADDR_WRITE_OTA, up.write_page_count);

        eeprom_write_byte((void *)ADDR_EE_UPDATE + offsetof(Update_t, main_page_count), 0);
        eeprom_write_byte((void *)ADDR_EE_UPDATE + offsetof(Update_t, write_page_count), 0);
        eeprom_busy_wait();

        // 24
        wdt_soft_reset();
    }
*/
    DDRB = _BV(PB5);

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
}

