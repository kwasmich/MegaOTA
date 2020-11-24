//
//  update.h
//  MegaOTA
//
//  Created by Michael Kwasnicki on 17.12.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//


#ifndef update_h
#define update_h

#include <avr/io.h>
#include <stdint.h>



#define ADDR_EE_UPDATE (E2END + 1 - sizeof(Update_t))



typedef struct {
    uint16_t main_crc;
    uint16_t write_crc;
    uint16_t boot_crc;
    uint8_t main_page_count;
    uint8_t write_page_count;
    uint8_t boot_page_count;
    uint8_t lfuse;
    uint8_t hfuse;
    uint8_t efuse;
    uint8_t lock;
    uint8_t signature[3];
} __attribute__((packed)) Update_t;



typedef struct {
    uint16_t base_address;

    union {
        uint8_t data[SPM_PAGESIZE];
        uint16_t word[SPM_PAGESIZE / 2];
    };
} update_page_t;



void update_page_add(update_page_t * const page, const uint8_t len, uint8_t data[static len], const uint16_t in_OFFSET);
void update_write_page(const update_page_t * const update_block) __attribute__((section(".write")));




#endif /* update_h */
