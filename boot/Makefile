MCU = atmega328p
MCU_DUDE = atmega328p
PROGRAMMER=usbtiny
#PROGRAMMER=avrispmkII
OBJCOPY = avr-objcopy
CC = avr-gcc
AVRSIZE = avr-size

CFLAGS = -Wall -Wno-main -Os -MMD -mmcu=$(MCU) -DF_CPU=16000000UL -I../shared/
LDFLAGS = -mrelax

# strip dead code
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections -Wl,--print-gc-sections \
						     -Wl,--undefined=main_version \
							 -Wl,--undefined=write_version \
							 -Wl,--undefined=boot_version

# link time optimization requires everything that is supposed to stay being flagged with __attribute__((used))
# CFLAGS += -flto
# LDFLAGS += -flto

# skip interrupt vector table
LDFLAGS += -nostartfiles


SOURCES = boot.c\
		  ../shared/i2c/i2c.c\
		  ../shared/i2c/at24cxx.c\
		  ../shared/spi/spi.c\
		  ../shared/spi/sst25vfxx.c
OBJECTS = $(SOURCES:%.c=%.o)
DEPS    = $(SOURCES:%.c=%.d)
EXECUTABLE = boot

MAIN_START      = 0x0000
MAIN_OTA_START  = 0x3800
WRITE_OTA_START = 0x7000
BOOT_OTA_START  = 0x7400
BOOT_START      = 0x7800
WRITE_START     = 0x7c00
FLASH_END       = 0x8000
MAIN_END	    = $(shell printf "0x%04x\n" $$(($(MAIN_OTA_START) - 1)))
MAIN_OTA_END    = $(shell printf "0x%04x\n" $$(($(WRITE_OTA_START) - 1)))
WRITE_OTA_END   = $(shell printf "0x%04x\n" $$(($(BOOT_OTA_START) - 1)))
BOOT_OTA_END    = $(shell printf "0x%04x\n" $$(($(BOOT_START) - 1)))
BOOT_END        = $(shell printf "0x%04x\n" $$(($(WRITE_START) - 1)))
WRITE_END       = $(shell printf "0x%04x\n" $$(($(FLASH_END) - 1)))


all: $(SOURCES) $(EXECUTABLE).hex


$(EXECUTABLE).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS)  -Wl,--section-start=.main=$(MAIN_START) \
								-Wl,--section-start=.main_guard=$(MAIN_END) \
								-Wl,--section-start=.main_ota=$(MAIN_OTA_START) \
								-Wl,--section-start=.main_ota_guard=$(MAIN_OTA_END) \
								-Wl,--section-start=.write_ota=$(WRITE_OTA_START) \
								-Wl,--section-start=.write_ota_guard=$(WRITE_OTA_END) \
								-Wl,--section-start=.boot_ota=$(BOOT_OTA_START) \
								-Wl,--section-start=.boot_ota_guard=$(BOOT_OTA_END) \
								-Wl,--section-start=.text=$(BOOT_START) \
								-Wl,--section-start=.boot_guard=$(BOOT_END) \
								-Wl,--section-start=.write=$(WRITE_START) \
								-Wl,--section-start=.write_guard=$(WRITE_END) \
								-o $@ $^


$(EXECUTABLE).hex: $(EXECUTABLE).elf
	$(OBJCOPY) -O ihex $< $@
	$(AVRSIZE) $<


%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	# $(CC) $(CFLAGS) -S -o $@.s -c $<
	# $(CC) $(CFLAGS) -E -o $@.pre -c $<


flash: all
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U flash:w:$(EXECUTABLE).elf:e
	# -U flash:w:../main/main.elf:e


fuse:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U lfuse:w:0xFF:m -U hfuse:w:0xD2:m -U efuse:w:0xFA:m -U lock:w:0x3F:m


dump:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U flash:r:dump_fash.hex:i
	# -U eeprom:r:dump_eeprom.hex:i


disassembly:
	avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D dump_fash.hex


clean:
	rm -f $(OBJECTS) $(DEPS)
	rm -f *.elf
	rm -f *.hex


-include $(DEPS)
