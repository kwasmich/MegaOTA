#ifndef sst25vfxx_h
#define sst25vfxx_h

#include "config.h"

#include <stdbool.h>
#include <stdint.h>



#ifndef SPI_EEPROM_24BIT
typedef uint16_t Address_t;
#else
typedef uint32_t Address_t;
#endif



void spi_eeprom_init();

void spi_eeprom_read_start(Address_t a);
uint8_t spi_eeprom_read_next();
void spi_eeprom_read_stop();
uint8_t spi_eeprom_read_byte(Address_t a);

void spi_eeprom_write_start(Address_t a, uint8_t data);
void spi_eeprom_write_next(uint8_t data);
void spi_eeprom_write_stop();
void spi_eeprom_write_byte(Address_t a, uint8_t data);

uint8_t spi_eeprom_manufacturer_id();
uint8_t spi_eeprom_device_id();

void spi_eeprom_erase_block(Address_t a);
void spi_eeprom_erase_sector(Address_t a);
void spi_eeprom_erase_chip();

void spi_eeprom_write_protect(bool protect);



#endif
