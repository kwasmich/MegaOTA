//
//  ws2811.h
//  MegaNeoPixel
//
//  Created by Michael Kwasnicki on 20.07.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#ifndef ws2811_h
#define ws2811_h


#include <stdint.h>


#ifndef WS2811_PORT
#   define WS2811_PORT PORTB
#endif


void ws2811_init(void);
void ws2811_deinit(void);

void ws2811_commit_bitbang(const uint8_t *values, uint8_t size, uint8_t bit);
void ws2811_commit_pwm(const uint8_t *values, uint8_t size);


#endif /* ws2811_h */
