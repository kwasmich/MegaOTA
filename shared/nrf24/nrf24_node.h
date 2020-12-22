//
//  nrf24_node.h
//  MegaOTA
//
//  Created by Michael Kwasnicki on 22.12.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#ifndef nrf24_node_h
#define nrf24_node_h


#include "nrf24_packets.h"


void nrf24_node_route(const uint8_t in_LENGTH, RawPacket_t * const in_PACKET);



#endif /* nrf24_node_h */
