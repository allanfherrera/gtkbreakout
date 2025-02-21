CC = gcc
CFLAGS = -O2
SOURCEFILE = gtkbreakout.c
OUTPUT = gtkbreakout
LIBRARIES = $(shell pkg-config --cflags --libs gtk+-3.0) -lm

$(OUTPUT): $(SOURCEFILE)
$(CC) $(CFLAGS) $(SOURCEFILE) -o $(OUTPUT) $(LIBRARIES)

clean:
rm -f $(OUTPUT)
