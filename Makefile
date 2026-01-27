CC := gcc
SRC := game.c
OUT := game

.PHONY: $(OUT)

CFLAGS += -Wall -Wextra

ifeq ($(OS),Windows_NT)
    RAYLIB_DIR := ./libs/raylib-5.5_win64_mingw-w64
    CFLAGS  += -I$(RAYLIB_DIR)/include
    LDFLAGS += -L$(RAYLIB_DIR)/lib
    LDLIBS  += -lraylib -lgdi32 -lwinmm
else
    RAYLIB_DIR := ./libs/raylib-5.5_linux_amd64
    CFLAGS  += -I$(RAYLIB_DIR)/include
    # Using the absolute/relative path to the .a file directly
    LIBRAYLIB := $(RAYLIB_DIR)/lib/libraylib.a
    LDLIBS  += $(LIBRAYLIB) -lGL -lm -lpthread -ldl -lrt
endif

# Ensure the output is rebuilt if the source or the library changes
$(OUT):
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(OUT)
