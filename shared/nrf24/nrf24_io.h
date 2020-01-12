//
//  nrf24_io.h
//  TinyRF24
//
//  Created by Michael Kwasnicki on 09.01.18.
//  Copyright © 2018 Michael Kwasnicki. All rights reserved.
//

#ifndef nrf24_io_h
#define nrf24_io_h


#include "nrf24_register.h"

#include <stdbool.h>
#include <stdint.h>


extern nrf24_register_status_u s_nrf24_io_status;
extern volatile bool s_nrf24_io_irq;


void nrf24_io_init(void);
void nrf24_io_command(const uint8_t in_COMMAND);
uint8_t nrf24_io_command_1(const uint8_t in_COMMAND, uint8_t in_DATA);
void nrf24_io_command_n(const uint8_t in_COMMAND, const uint8_t in_LENGTH, uint8_t in_out_payload[static const in_LENGTH]);
void nrf24_io_ce_pulse();
void nrf24_io_ce_hi();
void nrf24_io_ce_lo();


#endif /* nrf24_io_h */
