# Makefile for suckless minesweeper
# Minimal build system following suckless philosophy

CC = cc
CFLAGS = -Wall -Wextra -std=c99 -O2 -pedantic
LDFLAGS =
TARGET = minesweeper
SRC = minesweeper.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

# Convenience builds for different board sizes
small: CFLAGS += -DWIDTH=9 -DHEIGHT=9 -DMINES=10
small: $(TARGET)
	mv $(TARGET) minesweeper_small

medium: CFLAGS += -DWIDTH=16 -DHEIGHT=16 -DMINES=40
medium: $(TARGET)
	mv $(TARGET) minesweeper_medium

large: CFLAGS += -DWIDTH=30 -DHEIGHT=16 -DMINES=99
large: $(TARGET)
	mv $(TARGET) minesweeper_large

clean:
	rm -f minesweeper minesweeper_small minesweeper_medium minesweeper_large

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: clean install small medium large
