//
//  lcd_io.h
//  MegaOTA
//
//  Created by Michael Kwasnicki on 06.04.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#ifndef lcd_io_h
#define lcd_io_h


#include <stdint.h>



void lcd_io_send_command(uint8_t cmd);
void lcd_io_send_data(uint8_t data);
void lcd_io_clear_display(void);
void lcd_io_return_home(void);

void lcd_io_init(void);
void lcd_io_deinit(void);



#endif /* lcd_io_h */
