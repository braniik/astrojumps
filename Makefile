CC     = gcc
SRC    = $(wildcard src/*.c)
OBJS   = $(patsubst src/%.c, build/%.o, $(SRC))
CFLAGS = -Wall -Wextra -Iinclude
LIBS   = -lraylib -lm -ldl -lpthread

.PHONY: all debug clean

all: build $(OBJS)
	$(CC) $(OBJS) -o whirlybird $(LIBS)

debug: CFLAGS += -DDEBUG_BUILD -g
debug: build $(OBJS)
	$(CC) $(OBJS) -o whirlybird $(LIBS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build whirlybird
