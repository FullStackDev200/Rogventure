CC := gcc
SRC := game.c
OUT := game

CFLAGS += -Wall -Wextra

ifeq ($(OS),Windows_NT)
    RAYLIB_DIR := ./libs/raylib-5.5_win64_mingw-w64
    CFLAGS  += -I$(RAYLIB_DIR)/include
    LDFLAGS += -L$(RAYLIB_DIR)/lib
    LDLIBS  += -lraylib -lgdi32 -lwinmm
else
    RAYLIB_DIR := ./libs/raylib-5.5_linux_amd64
    CFLAGS  += -I$(RAYLIB_DIR)/include
    LDFLAGS += -L$(RAYLIB_DIR)/lib
    LDLIBS  += -lraylib -lGL -lm -lpthread -ldl -lrt
endif

$(OUT): $(SRC)
	$(CC) $(SRC) -o $@ $(CFLAGS) $(LDFLAGS) $(LDLIBS)
main: 
