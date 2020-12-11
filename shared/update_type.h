//
//  update.h
//  MegaOTA
//
//  Created by Michael Kwasnicki on 17.12.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//


#ifndef update_type_h
#define update_type_h

#include <stdbool.h>
#include <stdint.h>


typedef struct {
    uint16_t main_crc;
    uint16_t write_crc;
    uint16_t boot_crc;
    uint8_t lfuse;
    uint8_t hfuse;
    uint8_t efuse;
    uint8_t lock;
    uint8_t signature[3];
    bool prog_update;
    bool boot_update;
} __attribute__((packed)) Update_t;


#endif /* update_type_h */
