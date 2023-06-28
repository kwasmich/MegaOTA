//
//  update.h
//  MegaOTA
//
//  Created by Michael Kwasnicki on 17.12.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//


#ifndef update_h
#define update_h

#include "update_type.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>



#define ADDR_EE_UPDATE (E2END + 1 - sizeof(Update_t))


typedef struct {
    uint16_t base_address;

    union {
        uint8_t data[SPM_PAGESIZE];
        uint16_t word[SPM_PAGESIZE / 2];
    };
} update_page_t;



void update_page_add(update_page_t * const page, const uint8_t len, uint8_t data[static len], const uint16_t in_OFFSET);
bool update_write_ota_page(const update_page_t * const update_block);

#ifndef BOOTLOADER
void update_write_page(const update_page_t * const update_block) __attribute__((section(".write")));
#else
void update_write_page(const update_page_t * const update_block);
#endif


#endif /* update_h */
