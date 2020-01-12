//
//  nrf24_end_point.c
//  MegaRF24
//
//  Created by Michael Kwasnicki on 12.01.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#include "nrf24_end_point.h"

#include "nrf24.h"



void nrf24_end_point_init() {
    nrf24_init();
}



void nrf24_end_point_loop() {
    nrf24_loop();
}
