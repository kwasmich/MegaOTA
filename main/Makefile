include ../Makefile.common

EXECUTABLE=main


all: $(SOURCES) $(EXECUTABLE).hex $(EXECUTABLE).eep


$(EXECUTABLE).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,--section-start=.text=$(MAIN_START) -o $@ $^


$(EXECUTABLE).hex: $(EXECUTABLE).elf
	$(OBJCOPY) -R .eeprom -O ihex $< $@
	$(AVRSIZE) $<


$(EXECUTABLE).eep: $(EXECUTABLE).elf
	$(OBJCOPY) -j .eeprom -O ihex $< $@


%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	# $(CC) $(CFLAGS) -S -o $@.s -c $<
	# $(CC) $(CFLAGS) -E -o $@.pre -c $<


flash: all
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U flash:w:$(EXECUTABLE).elf:e -U flash:w:../boot/boot.elf:e


dump:
	avrdude -c $(PROGRAMMER) -p $(MCU_DUDE) -B 1MHz -U flash:r:dump_fash.hex:i -U eeprom:r:dump_eeprom.hex:i


serial:
	minicom -c on -D $(PORT)



disassembly:
	avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D dump_fash.hex


clean:
	rm -f $(OBJECTS) $(DEPS)
	rm -f *.elf
	rm -f *.hex


-include $(DEPS)
