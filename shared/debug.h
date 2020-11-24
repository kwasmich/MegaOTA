//
//  debug.h
//  MegaOTA
//
//  Created by Michael Kwasnicki on 27.01.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#ifndef debug_h
#define debug_h

#include <stdint.h>


// memory layout
// 0x100
// .data
// .bss
// heap
// free mem
// stack
// 0x8ff

// .data initialized static variables
extern uint16_t __data_start;
extern uint16_t __data_end;

// .bss  uninitialized static variables
extern uint16_t __bss_start;
extern uint16_t __bss_end;

// heap end variable
extern void *__brkval;

// ???
extern uint16_t __noinit_start;
extern uint16_t __noinit_end;
extern uint16_t __heap_start;

// stack
// SP macro


void debug_dump_pgm(void);
void debug_dump_mem(void);
void debug_clear_mem(void);
void debug_dump_eep(void);
void debug_dump_signature(void);
void debug_dump_fuse(void);


#endif /* debug_h */
