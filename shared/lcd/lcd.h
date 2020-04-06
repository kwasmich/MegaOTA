//
//  lcd.h
//  MegaOTA
//
//  Created by Michael Kwasnicki on 05.04.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#ifndef lcd_h
#define lcd_h


#include "lcd_command.h"

#include <stdbool.h>
#include <stdint.h>



//void lcd_clear_display(void);
//void lcd_carriage_return(void);
//void lcd_entry_mode(lcd_dir_e dir, bool shift);
//void lcd_display_cursor(bool d, bool c, bool b);
//void lcd_cursor_shift(bool)


void lcd_init(void);
void lcd_deinit(void);



#endif /* lcd_h */
