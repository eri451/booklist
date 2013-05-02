CC=clang
objects=$(patsubst %.c, %.o, $(wildcard src/*.c))

all: storelist

storelist: $(objects)
	$(CC) -g -v -O0 -o $@ $(objects)

clean: $(objects)
	rm $(objects)

%.o: %.c
	$(CC) -c -g -o $@ $<
