#define EXTERNAL_MEMORY_I2C 1
#define EXTERNAL_MEMORY_SPI 2

//#undef EXTERNAL_MEMORY
//#define EXTERNAL_MEMORY EXTERNAL_MEMORY_I2C
//#define EXTERNAL_MEMORY EXTERNAL_MEMORY_SPI

#undef I2C_SPEED

#undef SPI_EEPROM_24BIT

#define NUM_PAGES       ((FLASHEND + 1UL) / SPM_PAGESIZE)                       // 256
#define NUM_WRITE_PAGES 8
#define NUM_BOOT_PAGES  8
