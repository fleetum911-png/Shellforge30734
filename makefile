CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -Iinclude

LDFLAGS = -lreadline

TARGET = shellforge

SRC = src/history.c src/lexer.c src/main.c src/token.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
