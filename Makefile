CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.c hu01_decompressor.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=hu01

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@