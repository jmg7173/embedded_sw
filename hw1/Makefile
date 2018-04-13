CC=arm-none-linux-gnueabi-gcc
CFLAGS=-static -o
SOURCES=main.c in_proc.c clock.c counter.c draw.c device_helper.c editor.c out_proc.c mod_proc.c push_helper.c binary_game.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE = hw1_20141578.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $@

.c.o:
	$(CC) -c $< 

clean:
	rm -f *.o $(EXECUTABLE)
