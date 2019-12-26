//
//  ihex.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 26.12.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#include "ihex.h"

#include <string.h>



static uint8_t parse_nibble(const char c) {
    uint8_t nibble;

    if (c <= '9') {
        nibble = (c & 0x3F) - '0';
    } else {
        nibble = (c & 0x47) - ('A' - 10);
    }

    return nibble;
}



bool ihex_parse_async(ihex_state * const in_out_state, const char in_C) {
    // printf("%c", in_C);

    if (in_C == ':') {
        in_out_state->_state = 1;
        in_out_state->_data_cnt = 0;
        in_out_state->_hi = true;
        in_out_state->chksum_valid = false;
        return false;
    }

    switch (in_out_state->_state) {
        case 1:
        case 2:
            in_out_state->len <<= 4;
            in_out_state->len += parse_nibble(in_C);
            in_out_state->_state++;

            if (in_out_state->_state == 3 && in_out_state->len > IHEX_DATA_MAX) {
                in_out_state->_state = 13;
                return true;
            }

            break;

        case 3:
        case 4:
        case 5:
        case 6:
            in_out_state->offset <<= 4;
            in_out_state->offset += parse_nibble(in_C);
            in_out_state->_state++;
            break;

        case 7:
        case 8:
            in_out_state->type <<= 4;
            in_out_state->type += parse_nibble(in_C);
            in_out_state->_state++;
            break;

        case 9:
            in_out_state->data[in_out_state->_data_cnt] <<= 4;
            in_out_state->data[in_out_state->_data_cnt] += parse_nibble(in_C);
            in_out_state->_hi = !in_out_state->_hi;
            in_out_state->_data_cnt += !!in_out_state->_hi;

            if (in_out_state->_data_cnt == in_out_state->len) {
                in_out_state->_state++;
            }

            break;

        case 10:
        case 11:
            in_out_state->chksum <<= 4;
            in_out_state->chksum += parse_nibble(in_C);
            in_out_state->_state++;

            if (in_out_state->_state == 12) {
                uint8_t sum = in_out_state->len + in_out_state->type + in_out_state->chksum;
                sum += (in_out_state->offset >> 8) + in_out_state->offset;

                for (uint8_t i = 0; i < in_out_state->len; i++) {
                    sum += in_out_state->data[i];
                }

                in_out_state->chksum_valid = sum == 0;
                return true;
            }

            break;

        default:
            break;
    }

    return false;
};
