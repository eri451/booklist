CC=gcc
objects=$(patsubst %.c, %.o, $(wildcard src/*.c))

all: booklist

booklist: $(objects)
	$(CC) -g -O0 -o $@ $(objects) `pkg-config --libs gtk+-2.0`

clean: $(objects)
	rm $(objects)

%.o: %.c
	$(CC) -c -g -o $@ $< `pkg-config --cflags gtk+-2.0`
