//
//  ihex.h
//  MegaOTA
//
//  Created by Michael Kwasnicki on 26.12.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#ifndef ihex_h
#define ihex_h

#include "config.h"

#include <stdbool.h>
#include <stdint.h>



#ifndef IHEX_DATA_MAX
#   define IHEX_DATA_MAX 16
#endif



typedef struct {
    uint8_t  len;
    uint16_t offset;
    uint8_t  type;
    uint8_t  data[IHEX_DATA_MAX];
    uint8_t  chksum;
    bool     chksum_valid;

    // internal state
    uint8_t  _state;
    uint8_t  _data_cnt;
    bool     _hi;
} ihex_state_t;



bool ihex_parse_async(ihex_state_t * const in_out_state, const char in_C);



#endif /* ihex_h */
