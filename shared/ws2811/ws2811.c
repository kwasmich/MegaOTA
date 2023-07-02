//
//  ws2811.c
//  MegaNeoPixel
//
//  Created by Michael Kwasnicki on 02.07.23.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#include "ws2811.h"


#include <util/delay.h>



void ws2811_commit() {
    _delay_loop_2(1000);    // reset delay is 0.5ms = 1000 * CPU cycles @ 8MHz
}
