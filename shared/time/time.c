//
//  time.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 17.07.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//


#include "time.h"

#include "config.h"

#include "time/wdt_time.h"



volatile uint32_t s_time_ms = 0;



void time_init() {
#ifdef TIME_WDT
    time_wdt_init();
#else
#   error no time source defined
#endif
}
