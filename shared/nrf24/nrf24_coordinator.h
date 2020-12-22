//
//  nrf24_coordinator.h
//  MegaRF24
//
//  Created by Michael Kwasnicki on 12.01.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#ifndef nrf24_coordinator_h
#define nrf24_coordinator_h


#include <stdbool.h>
#include <stdint.h>



void nrf24_coordinator_init(void);
bool nrf24_coordinator_rx(uint8_t * const out_len, uint8_t out_payload[static const 32]);



#endif /* nrf24_coordinator_h */
