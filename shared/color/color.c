
//
//  color.c
//  MegaOTA
//
//  Created by Michael Stocker on 20.06.23.
//  Copyright © 2023 Michael Stocker. All rights reserved.
//

#include "color.h"



static uint8_t gamma8(uint8_t in) {
    uint16_t pow = in * in;
    return pow >> 8;
}



rgb8_t hsv2rgb(uint16_t hue, uint8_t sat, uint8_t val) {
    uint16_t r, g, b;
    uint32_t h = (hue * 1530UL) / 65536UL;
    hue = h & 0xFFFF;

    switch (hue) {
        case 0 ... 254:
            r = 255;
            g = hue & 0xFF;
            b = 0;
            break;

        case 255 ... 509:
            r = 255 - ((hue + 1) & 0xFF);
            g = 255;
            b = 0;
            break;

        case 510 ... 764:
            r = 0;
            g = 255;
            b = (hue + 2) & 0xFF;
            break;

        case 765 ... 1019:
            r = 0;
            g = 255 - ((hue + 3) & 0xFF);
            b = 255;
            break;

        case 1020 ... 1274:
            r = (hue + 4) & 0xFF;
            g = 0;
            b = 255;
            break;

        case 1275 ... 1529:
            r = 255;
            g = 0;
            b = 255 - ((hue + 5) & 0xFF);
            break;
    }
    
    if (sat < 255) {
        r = ((r * (sat + 1)) >> 8) + 255 - sat;
        g = ((g * (sat + 1)) >> 8) + 255 - sat;
        b = ((b * (sat + 1)) >> 8) + 255 - sat;
    }

    if (val < 255) {
        r = (r * (val + 1)) >> 8;
        g = (g * (val + 1)) >> 8;
        b = (b * (val + 1)) >> 8;
    }

    r = gamma8(r);
    g = gamma8(g);
    b = gamma8(b);
        
    return (rgb8_t){ .r = r, .g = g, .b = b };
}
