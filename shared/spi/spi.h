#ifndef spi_h
#define spi_h


#include <stdint.h>



void spi_init(void);
void spi_deinit(void);
uint8_t spi_exchange(uint8_t data);


// failed attempt to decrease size
// uint8_t spi_read();
// void spi_write(uint8_t data);



#endif
