# Suckless Minesweeper

A minimalist implementation of the classic Minesweeper game inspired by the Suckless and Less Retarded Software (LRS) philosophies, using a Battleship-style board approach for simplicity and reuse of proven patterns.

## Philosophy

This implementation follows principles of extreme minimalism and the suckless way of software development:

- **Minimalist**: Single compilation unit, no external dependencies beyond standard C library
- **Simple**: Does one thing well - provides Minesweeper gameplay
- **Reusable**: Uses Battleship-style board approach for proven simplicity
- **Hackable**: Easy to modify and customize by editing source code
- **Self-contained**: Just compile and run, no configuration files needed
- **Portable**: Written in C99 for maximum compatibility

## Features

- Classic Minesweeper gameplay
- Text-based interface using ASCII characters
- Configurable board size and mine count via compile-time constants
- Safe first-click (mines are placed after first move)
- Flood fill for revealing empty areas
- Flagging capability for suspected mines

## Building

```bash
# Default 9x9 board with 10 mines
make

# Different board sizes
make small    # 9x9 with 10 mines
make medium   # 16x16 with 40 mines  
make large    # 30x16 with 99 mines

# Custom board
cc -DWIDTH=20 -DHEIGHT=20 -DMINES=50 minesweeper_suckless.c -o minesweeper
```

## Controls

Commands use row-first ordering (row, column):

- `r row col`: Reveal cell at coordinates (e.g., `r 2 3` reveals row 2, column 3)
- `f row col`: Toggle flag at coordinates (e.g., `f 4 1` flags row 4, column 1)
- `q`: Quit game

## Customization

Edit these compile-time constants in the source or define them during compilation with `-D` flags:

- `WIDTH`: Board width (default 9)
- `HEIGHT`: Board height (default 9) 
- `MINES`: Number of mines (default 10)

## License

This software is released into the public domain using the Unlicense (CC0 + patent waivers). There are no restrictions on use, modification, or distribution. This maximizes freedom for all users without any copyright claims or artificial restrictions.
