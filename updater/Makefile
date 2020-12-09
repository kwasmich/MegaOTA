include ../Makefile.config

PORT=`find /dev/cu.wchusbserial* /dev/cu.usbserial* /dev/cu.usbmodem* -print -quit`


CFLAGS = -Wall -Wpedantic -Os -MMD -I../shared/
LDFLAGS = -mrelax

# strip dead code
CFLAGS += -ffunction-sections -fdata-sections -fshort-enums \
	-DADDR_MAIN_START=$(MAIN_START) \
	-DADDR_MAIN_END="$(MAIN_END) + 2" \
	-DADDR_MAIN_OTA_START=$(MAIN_OTA_START) \
	-DADDR_MAIN_OTA_END="$(MAIN_OTA_END) + 2" \
	-DADDR_WRITE_START=$(WRITE_START) \
	-DADDR_WRITE_END="$(WRITE_END) + 2UL" \
	-DADDR_WRITE_OTA_START=$(WRITE_OTA_START) \
	-DADDR_WRITE_OTA_END="$(WRITE_OTA_END) + 2" \
	-DADDR_BOOT_START=$(BOOT_START) \
	-DADDR_BOOT_END="$(BOOT_END) + 2" \
	-DADDR_BOOT_OTA_START=$(BOOT_OTA_START) \
	-DADDR_BOOT_OTA_END="$(BOOT_OTA_END) + 2" \
	-DLFUSE=$(LFUSE) \
	-DHFUSE=$(HFUSE) \
	-DEFUSE=$(EFUSE) \
	-DLOCKFUSE=$(LOCKFUSE)

# link time optimization requires everything that is supposed to stay being flagged with __attribute__((used))
# CFLAGS += -flto
# LDFLAGS += -flto


SOURCES=$(wildcard *.c) \
	../shared/crypto/crc.c
OBJECTS=$(SOURCES:%.c=%.o)
DEPS=$(sort $(patsubst %, %.deps, $(OBJECTS)))


EXECUTABLE=updater


all: $(SOURCES) $(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	# $(CC) $(CFLAGS) -S -o $@.s -c $<
	# $(CC) $(CFLAGS) -E -o $@.pre -c $<


clean:
	rm -f $(OBJECTS) $(DEPS)
	rm -f $(EXECUTABLE)


-include $(DEPS)
