TARGET = mqtt2modbus
CC = cc
CFLAGS = -m64 -Wall -pedantic -O2 `pkg-config --cflags libmodbus` `pkg-config --cflags libmosquitto`
LDFLAGS = -m64 
LIBS = `pkg-config --libs libmosquitto` `pkg-config --libs libmodbus`

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
