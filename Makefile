CC      = gcc
TARGET  = whirlybird
SRC_DIR = src
INC_DIR = include
BLD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BLD_DIR)/%.o, $(SRCS))

CFLAGS = -Wall -Wextra -I$(INC_DIR)

# Detect OS
ifeq ($(OS), Windows_NT)
    TARGET_BIN = $(TARGET).exe
    LDFLAGS    = -lraylib -lopengl32 -lgdi32 -lwinmm
    MKDIR      = if not exist $(BLD_DIR) mkdir $(BLD_DIR)
    RM         = del /Q
else
    TARGET_BIN = $(TARGET)
    LDFLAGS    = -lraylib -lm -ldl -lpthread
    MKDIR      = mkdir -p $(BLD_DIR)
    RM         = rm -f
endif

.PHONY: all debug clean

all: CFLAGS += -O2
all: $(BLD_DIR) $(TARGET_BIN)

debug: CFLAGS += -DDEBUG_BUILD -g
debug: $(BLD_DIR) $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BLD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BLD_DIR):
	$(MKDIR)

clean:
	$(RM) $(BLD_DIR)/*.o $(TARGET_BIN)
