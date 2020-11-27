PORT=`find /dev/cu.wchusbserial* /dev/cu.usbserial* /dev/cu.usbmodem* -print -quit`


CFLAGS = -Wall -Wpedantic -Os -MMD -I../shared/
LDFLAGS = -mrelax

# strip dead code
CFLAGS += -ffunction-sections -fdata-sections -fshort-enums

# link time optimization requires everything that is supposed to stay being flagged with __attribute__((used))
# CFLAGS += -flto
# LDFLAGS += -flto


SOURCES=$(wildcard *.c)
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
