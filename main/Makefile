MCU=atmega328p
MCU_DUDE=atmega328p
OBJCOPY=avr-objcopy
CC=avr-gcc
AVRSIZE=avr-size
CFLAGS=-Wall -Wno-main -Os -mmcu=$(MCU) -DF_CPU=16000000UL -I../shared/
LDFLAGS=-mrelax

# strip dead code
CFLAGS+=-ffunction-sections -fdata-sections
LDFLAGS+=-Wl,--gc-sections -Wl,--print-gc-sections

# link time optimization
# CFLAGS+=-flto
# LDFLAGS+=-flto

# skip interrupt vector table
# LDFLAGS+=-nostartfiles


SOURCES=main.c
OBJECTS=$(SOURCES:%.c=%.o)
EXECUTABLE=main


all: $(SOURCES) $(EXECUTABLE).hex


$(EXECUTABLE).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS)  -o $@ $^


$(EXECUTABLE).hex: $(EXECUTABLE).elf
	$(OBJCOPY) -O ihex $< $@
	$(AVRSIZE) $<


%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	# $(CC) $(CFLAGS) -S -o $@.s -c $<
	# $(CC) $(CFLAGS) -E -o $@.pre -c $<


flash: all
	avrdude -c usbtiny -p $(MCU_DUDE) -B 250kHz -U flash:w:$(EXECUTABLE).elf:e
	

dump:
	avrdude -c usbtiny -p $(MCU_DUDE) -B 250kHz -U flash:r:dump_fash.hex:i -U eeprom:r:dump_eeprom.hex:i


disassembly:
	avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D $(EXECUTABLE).hex
	# avr-objdump -s -m avr5 -D dump_fash.hex


clean:
	rm -f $(OBJECTS)
	rm -f *.elf
	rm -f *.hex
