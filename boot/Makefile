include ../Makefile.config
include ../Makefile.common

LDFLAGS += -nostartfiles


SOURCES = boot.c\
		../shared/crypto/crc.c \
		../shared/i2c/at24cxx.c \
		../shared/i2c/i2c.c \
		../shared/spi/spi.c \
		../shared/spi/sst25vfxx.c \
		../shared/section_guards.c \
#		../shared/uart/uart.c
OBJECTS = $(SOURCES:%.c=%.o)
DEPS    = $(SOURCES:%.c=%.d)
EXECUTABLE = boot

all: $(SOURCES) $(EXECUTABLE).hex $(EXECUTABLE).eep


$(EXECUTABLE).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,--section-start=.text=$(BOOT_START) -o $@ $^


$(EXECUTABLE).hex: $(EXECUTABLE).elf
	$(OBJCOPY) -j .text -j .data -j .boot_guard -O ihex $< $@
	$(AVRSIZE) $<


$(EXECUTABLE).eep: $(EXECUTABLE).elf
	$(OBJCOPY) -j .eeprom -O ihex $< $@


%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
# $(CC) $(CFLAGS) -S -o $@.s -c $<
# $(CC) $(CFLAGS) -E -o $@.pre -c $<


flash: all
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U flash:w:$(EXECUTABLE).elf:e
# -U flash:w:../main/main.elf:e


fuse:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U lfuse:w:0xFF:m -U hfuse:w:0xD2:m -U efuse:w:0xFA:m -U lock:w:0x3F:m


erase:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -e


dump:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U flash:r:dump.hex:i -U eeprom:r:dump.eep:i


disassembly:
	avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D dump_fash.hex


clean:
	rm -f $(OBJECTS) $(DEPS)
	rm -f *.elf
	rm -f *.hex


-include $(DEPS)
