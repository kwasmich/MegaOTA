include ../Makefile.common

#CFLAGS += -DBOOTLOADER
LDFLAGS += -T boot.ld -Wl,-Map=boot.map

SOURCES = boot.c \
		../shared/crypto/crc.c \
		../shared/i2c/at24cxx.c \
		../shared/i2c/i2c.c \
		../shared/spi/spi.c \
		../shared/spi/sst25vfxx.c \
		../shared/section_guards.c \
		../shared/update.c
#		../shared/uart/uart.c
OBJECTS = $(SOURCES:%.c=%.o)
DEPS    = $(SOURCES:%.c=%.d)
EXECUTABLE = boot

SOURCES_S = ../sections.s
OBJECTS_S = $(SOURCES_S:%.s=%.o)

all: $(SOURCES) $(EXECUTABLE).hex $(EXECUTABLE).eep


.PHONY: rebuild
# rebuild: CFLAGS=-Wall
rebuild: clean
rebuild: all


$(EXECUTABLE).elf: $(OBJECTS) $(OBJECTS_S)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


$(EXECUTABLE).hex: $(EXECUTABLE).elf
	$(OBJCOPY) -j .text -j .boot_version -j .write  -j .write_version -j .data -O ihex $< $@
	$(AVRSIZE) $<


$(EXECUTABLE).eep: $(EXECUTABLE).elf
	$(OBJCOPY) -j .eeprom -O ihex $< $@


%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
# $(CC) $(CFLAGS) -S -o $@.s -c $<
# $(CC) $(CFLAGS) -E -o $@.pre -c $<


%.o: %.s
	$(CC) $(CFLAGS) -o $@ -c $<


.PHONY: flash
flash: all
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B $(PRG_SPEED) -U flash:w:$(EXECUTABLE).elf:e
# -U flash:w:../main/main.elf:e


.PHONY: fuse
fuse:
	avr-objcopy -j .lock -O ihex --change-section-lma .lock=0 $(EXECUTABLE).elf lock.hex
	avr-objcopy -j .fuse -O ihex --change-section-lma .fuse=0 $(EXECUTABLE).elf fuses.hex
	srec_cat fuses.hex -Intel -crop 0x00 0x01 -offset  0x00 -O lfuse.hex -Intel
	srec_cat fuses.hex -Intel -crop 0x01 0x02 -offset -0x01 -O hfuse.hex -Intel
	srec_cat fuses.hex -Intel -crop 0x02 0x03 -offset -0x02 -O efuse.hex -Intel
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B $(PRG_SPEED) -U lfuse:w:lfuse.hex:i -U hfuse:w:hfuse.hex:i -U efuse:w:efuse.hex:i -U lock:w:lock.hex:i


.PHONY: dump_fuse
dump_fuse:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B $(PRG_SPEED) -U lfuse:r:-:i -U hfuse:r:-:i -U efuse:r:-:i -U lock:r:-:i


.PHONY: chip_erase
chip_erase:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B $(PRG_SPEED) -e


.PHONY: dump
dump:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B $(PRG_SPEED) -U flash:r:dump.hex:i -U eeprom:r:dump.eep:i


.PHONY: disassembly
disassembly:
	avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D dump_fash.hex


.PHONY: clean
clean:
	rm -f $(OBJECTS) $(DEPS)
	rm -f *.elf
	rm -f *.hex


-include $(DEPS)
