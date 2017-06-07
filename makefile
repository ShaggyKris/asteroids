TARGET = asteroids
LIBS = -lglut -lGLU -lGL -lm -lftgl 
CC = gcc
CFLAGS = -g -Wall -I/usr/include/FTGL -I/usr/include/freetype2

.PHONY: default run clean

default: run
all: default

OBJECTS = $(patsubst %.c, .objectFiles/%.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

.objectFiles/%.o: %.c $(HEADERS)
	mkdir -p objectFiles
	$(CC) $(CFLAGS) -c -w $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

run: $(TARGET)
	./asteroids

clean:
	-rm -f *.o
	-rm -f .objectFiles
	-rm -f $(TARGET)
