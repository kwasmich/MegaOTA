//
//  nrf24_x.h
//  MegaRF24
//
//  Created by Michael Kwasnicki on 04.01.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#ifndef nrf24_h
#define nrf24_h

#include <stdbool.h>
#include <stdint.h>


void nrf24_rx_start();
void nrf24_rx_stop();
void nrf24_tx(const uint8_t in_LENGTH, const uint8_t in_PAYLOAD[static const in_LENGTH]);
bool nrf24_rx(uint8_t *length, uint8_t payload[static const 32]);
void nrf24_init(void);
void nrf24_loop(void);
void nrf24_debug(void);



#endif /* nrf24_x_h */
