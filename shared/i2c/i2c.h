#ifndef i2c_h
#define i2c_h

#include <stdbool.h>
#include <stdint.h>



void i2c_init(void);
void i2c_deinit(void);
bool i2c_start(void);
void i2c_stop(void);
bool i2c_address(uint8_t SLA, bool R);
bool i2c_data(uint8_t DATA, bool R, bool ACK);



#endif
