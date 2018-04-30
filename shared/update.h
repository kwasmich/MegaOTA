#ifndef ota_h
#define ota_h

#include <stdint.h>



typedef struct {
    uint16_t main_crc;
    uint16_t write_crc;
    uint16_t boot_crc;
    uint8_t main_page_count;
    uint8_t write_page_count;
    uint8_t boot_page_count;
    uint8_t lfuse;
    uint8_t hfuse;
    uint8_t efuse;
    uint8_t lock;
    uint8_t signature[3];
} __attribute__((packed)) Update_t;



#endif
