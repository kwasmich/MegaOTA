#ifndef at24cxx_h
#define at24cxx_h

#include <stdbool.h>
#include <stdint.h>



uint8_t i2c_eeprom_read_byte(uint16_t ADDR);
uint8_t i2c_eeprom_read_next_byte();
void i2c_eeprom_read_block(uint8_t *out, uint8_t CNT, uint16_t ADDR);
bool i2c_eeprom_write_byte(uint16_t ADDR, uint8_t DATA);
bool i2c_eeprom_write_block(uint16_t ADDR, uint8_t *DATA, uint8_t CNT);



#endif
