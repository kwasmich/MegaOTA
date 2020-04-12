//
//  lcd.h
//  MegaOTA
//
//  Created by Michael Kwasnicki on 05.04.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#ifndef lcd_h
#define lcd_h


#include <stdint.h>



void lcd_clear_display(void);
void lcd_goto_line(uint8_t line);
void lcd_set_glyph5x8(uint8_t c, uint8_t data[static const 8]);
void lcd_set_glyph5x11(uint8_t c, uint8_t data[static const 11]);

void lcd_init(void);
void lcd_deinit(void);



#endif /* lcd_h */
