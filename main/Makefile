include ../Makefile.common


EXECUTABLE=main


all: $(SOURCES) $(EXECUTABLE).hex $(EXECUTABLE).eep

.PHONY: rebuild
# rebuild: CFLAGS=-Wall
rebuild: clean
rebuild: all

.PHONY: update
update: all update_main.bin update_write.bin update_boot.bin


$(EXECUTABLE).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,--section-start=.text=$(MAIN_START) -o $@ $^


$(EXECUTABLE).hex: $(EXECUTABLE).elf
	$(OBJCOPY) -j .text -j .data -j .main_version -j .write -j .write_version -O ihex $< $@
	$(AVRSIZE) $<


$(EXECUTABLE).eep: $(EXECUTABLE).elf
	$(OBJCOPY) -j .eeprom -O ihex --change-section-lma .eeprom=0 $< $@

update_main.bin: $(EXECUTABLE).elf
	$(OBJCOPY) -j .text -j .data -j .main_version --gap-fill 0xFF -O binary $< $@

update_write.bin: $(EXECUTABLE).elf
	$(OBJCOPY) -j .write -j .write_version --gap-fill 0xFF -O binary $< $@

update_boot.bin: ../boot/boot.elf
	$(OBJCOPY) -j .text -j .boot_version --gap-fill 0xFF -O binary $< $@


%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
# $(CC) $(CFLAGS) -S -o $@.s -c $<
# $(CC) $(CFLAGS) -E -o $@.pre -c $<


.PHONY: flash
flash: all
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U flash:w:$(EXECUTABLE).hex:i -U flash:w:../boot/boot.hex:i
# -U eeprom:w:$(EXECUTABLE).eep:i
# avrdude -c arduino -P $(PORT) -p $(MCU_DUDE) -b115200 -U flash:w:$(EXECUTABLE).hex:i
# -U eeprom:w:$(EXECUTABLE).eep:i


.PHONY: chip_erase
chip_erase:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B $(PRG_SPEED) -e


.PHONY: erase_eep
erase_eep:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B $(PRG_SPEED) -U eeprom:w:_empty.eep:i


.PHONY: dump_flash
dump_flash:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B $(PRG_SPEED) -U flash:r:dump.hex:i


.PHONY: dump_eep
dump_eep:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B $(PRG_SPEED) -U eeprom:r:dump.eep:i


.PHONY: dump_fuses
dump_fuse:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B $(PRG_SPEED) -U lfuse:r:-:i -U hfuse:r:-:i -U efuse:r:-:i -U lock:r:-:i


.PHONY: serial
serial:
	minicom -b 19200 -c on -D $(PORT)


.PHONY: disassembly
disassembly:
	avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
# avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
# avr-objdump -s -m avr5 -D dump_fash.hex


.PHONY: clean
clean:
	rm -f $(OBJECTS) $(DEPS)
	rm -f $(EXECUTABLE).eep
	rm -f $(EXECUTABLE).elf
	rm -f $(EXECUTABLE).hex
	-rm -r update_main.bin update_write.bin update_boot.bin


-include $(DEPS)
