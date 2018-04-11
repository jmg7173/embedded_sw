CC=arm-none-linux-gnueabi-gcc
CFLAGS=-static -o
SOURCES=main.c in_proc.c led.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE = hw1_20141578.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $@

.c.o:
	$(CC) -c $< $(CFLAGS) $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
