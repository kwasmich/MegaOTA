#define EXTERNAL_MEMORY_I2C 1
#define EXTERNAL_MEMORY_SPI 2

//#undef EXTERNAL_MEMORY
//#define EXTERNAL_MEMORY EXTERNAL_MEMORY_I2C
//#define EXTERNAL_MEMORY EXTERNAL_MEMORY_SPI


#define NUM_PAGES       ((FLASHEND + 1UL) / SPM_PAGESIZE)                       // 256
#define NUM_WRITE_PAGES 8
#define NUM_BOOT_PAGES  8


// I²C
//#define I2C_SPEED 400000
//#define I2C_EEPROM_ADDRESS 0x50
//#define I2C_EEPROM_PAGE_SIZE 128

// Intel HEX
//#define IHEX_DATA_MAX 32

// SPI
//#define SPI_STDOUT
//#define SPI_EEPROM_24BIT

// UART
#define UART_BAUD 19200
#define UART_RX_BUFFER_SIZE 4
#define UART_TX_BUFFER_SIZE 4
#define UART_STDOUT
