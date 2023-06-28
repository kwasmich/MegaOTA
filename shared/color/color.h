//
//  color.h
//  MegaOTA
//
//  Created by Michael Stocker on 20.06.23.
//  Copyright © 2023 Michael Stocker. All rights reserved.
//

#ifndef color_h
#define color_h


#include <stdint.h>


typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb8_t;


rgb8_t hsv2rgb(uint16_t hue, uint8_t sat, uint8_t val);


#endif /* color_h */
