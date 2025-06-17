CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = assembler

all: $(TARGET)

$(TARGET): assembler.c
	$(CC) $(CFLAGS) -o $(TARGET) assembler.c

clean:
	rm -f $(TARGET)

.PHONY: all clean 