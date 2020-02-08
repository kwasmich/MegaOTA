include ../Makefile.common

EXECUTABLE=main


all: $(SOURCES) $(EXECUTABLE).hex $(EXECUTABLE).eep


$(EXECUTABLE).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,--section-start=.text=$(MAIN_START) -o $@ $^


$(EXECUTABLE).hex: $(EXECUTABLE).elf
	$(OBJCOPY) -R .eeprom -O ihex $< $@
	$(AVRSIZE) $<


$(EXECUTABLE).eep: $(EXECUTABLE).elf
	$(OBJCOPY) -j .eeprom -O ihex --change-section-lma .eeprom=0 $< $@


%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	# $(CC) $(CFLAGS) -S -o $@.s -c $<
	# $(CC) $(CFLAGS) -E -o $@.pre -c $<


flash: all
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U flash:w:$(EXECUTABLE).hex:i
	# -U flash:w:../boot/boot.hex:i -U eeprom:w:$(EXECUTABLE).eep:i
	# avrdude -c arduino -P $(PORT) -p $(MCU_DUDE) -b115200 -U flash:w:$(EXECUTABLE).hex:i
	# -U eeprom:w:$(EXECUTABLE).eep:i


fuse:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U lfuse:r:-:i -U hfuse:r:-:i -U efuse:r:-:i -U lock:r:-:i
	# avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U lock:w:0x3F:m


erase:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -e


dump:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U flash:r:dump.hex:i -U eeprom:r:dump.eep:i
	# avrdude -c arduino -P $(PORT) -p $(MCU_DUDE) -B 1MHz -U flash:r:dump.hex:i -U eeprom:r:dump.eep:i


serial:
	minicom -c on -D $(PORT)



disassembly:
	avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D dump_fash.hex


clean:
	rm -f $(OBJECTS) $(DEPS)
	rm -f $(EXECUTABLE).eep
	rm -f $(EXECUTABLE).elf
	rm -f $(EXECUTABLE).hex


-include $(DEPS)
