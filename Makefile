CC      := gcc
TARGET  := whirlybird
SRCDIR  := src
INCDIR  := include
BUILDDIR:= build
SRCS    := $(wildcard $(SRCDIR)/*.c)
OBJS    := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))

RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS   := $(shell pkg-config --libs   raylib 2>/dev/null)

ifeq ($(RAYLIB_LIBS),)
    RAYLIB_CFLAGS := -I/usr/local/include
    RAYLIB_LIBS   := -L/usr/local/lib -lraylib
endif

CFLAGS  := -Wall -Wextra -std=c99 -O2 -I$(INCDIR) $(RAYLIB_CFLAGS)
LDFLAGS := $(RAYLIB_LIBS) -lm

.PHONY: all debug clean run

all: $(BUILDDIR) $(TARGET)

debug: CFLAGS += -g -DDEBUG_BUILD
debug: $(BUILDDIR) whirlybird_debug

whirlybird_debug: $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "✓  Debug build → ./whirlybird_debug"

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "✓  Build successful → ./$(TARGET)"

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILDDIR) $(TARGET) whirlybird_debug
