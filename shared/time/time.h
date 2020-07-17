//
//  time.h
//  MegaOTA
//
//  Created by Michael Kwasnicki on 17.07.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#ifndef time_h
#define time_h


#include <stdint.h>


extern volatile uint32_t s_time_ms;


void time_init(void);


#endif /* time_h */
