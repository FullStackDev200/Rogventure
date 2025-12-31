CC=gcc
CFLAGS=-MMD -Wall -Wextra -std=c17


main: 
	$(CC) $(CFLAGS) game.c -o game -I./raylib-5.5_linux_amd64/include/ -L./raylib-5.5_linux_amd64/lib/ -l:libraylib.a -lGL -lm -lpthread -ldl -lrt
