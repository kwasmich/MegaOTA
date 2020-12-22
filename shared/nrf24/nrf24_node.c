//
//  nrf24_node.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 22.12.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "nrf24.h"
#include "nrf24_node.h"

#include <stdlib.h>



void nrf24_node_route(const uint8_t in_LENGTH, RawPacket_t * const in_PACKET) {
    const div_t d = div(in_PACKET->dst, 6);
    const uint8_t pipe = d.rem;
    in_PACKET->dst = d.quot;
    in_PACKET->src = in_PACKET->src * 6 + pipe;
    nrf24_tx(in_LENGTH, (uint8_t *)in_PACKET);
}
