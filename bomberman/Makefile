CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude
SRC     = src/main.c src/game.c src/arena.c src/entities.c src/input.c src/score.c
OBJ     = $(SRC:.c=.o)
ifeq ($(OS),Windows_NT)
TARGET  = bomberman.exe
else
TARGET  = bomberman
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) scores.txt

.PHONY: all clean
