/*
 * minesweeper - classic minesweeper game using battleship-style board
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 *
 * Written by Janne Alexander Sebastian Rovio
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Default board dimensions and mine count - redefine with compiler flags if needed */
#ifndef WIDTH
#define WIDTH 9
#endif

#ifndef HEIGHT  
#define HEIGHT 9
#endif

#ifndef MINES
#define MINES 10
#endif

/* Compile-time checks */
#if MINES >= (WIDTH * HEIGHT - 1)
#error "Too many mines for board size"
#endif

/* Board cell states - using char values like battleship */
#define HIDDEN '.'     /* Hidden cell */
#define MINE '*'       /* Mine */
#define REVEALED '0'   /* Revealed empty (with adjacent mine count) */
#define FLAGGED 'F'    /* Flagged by player */
#define MISS 'X'       /* Miss marker (for display when game over) */

/* Game board */
static char board[HEIGHT][WIDTH];
static char display_board[HEIGHT][WIDTH];  /* What the player sees */
static int game_over = 0;
static int won = 0;
static int first_move = 1;

/* Count adjacent mines for a cell */
static int count_adjacent_mines(int x, int y)
{
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            
            if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                if (board[ny][nx] == MINE) count++;
            }
        }
    }
    return count;
}

/* Place mines after first move (to ensure first click is safe) */
static void place_mines(int safe_x, int safe_y)
{
    int mines_placed = 0;
    
    /* Place mines randomly avoiding safe cell */
    while (mines_placed < MINES) {
        int x = rand() % WIDTH;
        int y = rand() % HEIGHT;
        
        if ((x == safe_x && y == safe_y) || (board[y][x] == MINE)) continue;
        
        board[y][x] = MINE;
        mines_placed++;
    }
}

/* Reveal cell with iterative flood fill for empty areas (prevents stack overflow) */
static void reveal(int x, int y)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    if (display_board[y][x] != HIDDEN) return;  /* Already revealed or flagged */

    /* Place mines on first move to ensure safe first click */
    if (first_move) {
        place_mines(x, y);
        first_move = 0;
    }

    /* Check for mine */
    if (board[y][x] == MINE) {
        game_over = 1;
        display_board[y][x] = MINE;  /* Show the mine that was hit */
        return;
    }

    /* Use iterative flood fill to reveal empty areas */
    int queue[WIDTH * HEIGHT * 2];  /* Queue for BFS: x, y, x, y, ... */
    int front = 0;
    int rear = 0;
    
    /* Add initial position to queue */
    queue[rear++] = x;
    queue[rear++] = y;
    
    while (front < rear) {
        int curr_x = queue[front++];
        int curr_y = queue[front++];
        
        /* Only process if it's still hidden */
        if (display_board[curr_y][curr_x] != HIDDEN) continue;
        
        /* Mark as revealed with adjacent mine count */
        int mine_count = count_adjacent_mines(curr_x, curr_y);
        display_board[curr_y][curr_x] = (mine_count == 0) ? ' ' : ('0' + mine_count);
        
        /* If no adjacent mines, reveal neighbors too */
        if (mine_count == 0) {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;  /* Skip center */
                    
                    int nx = curr_x + dx;
                    int ny = curr_y + dy;
                    
                    if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                        if (display_board[ny][nx] == HIDDEN && board[ny][nx] != MINE) {
                            queue[rear++] = nx;
                            queue[rear++] = ny;
                        }
                    }
                }
            }
        }
    }
}

/* Toggle flag on cell */
static void flag(int x, int y)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    if (display_board[y][x] != HIDDEN && display_board[y][x] != FLAGGED) return;  /* Can't flag revealed cells */
    
    if (display_board[y][x] == HIDDEN) {
        display_board[y][x] = FLAGGED;
    } else if (display_board[y][x] == FLAGGED) {
        display_board[y][x] = HIDDEN;
    }
}

/* Check win condition */
static int check_win(void)
{
    int revealed_count = 0;
    int total_safe = WIDTH * HEIGHT - MINES;
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (display_board[y][x] != HIDDEN && display_board[y][x] != FLAGGED) {
                revealed_count++;
            }
        }
    }
    
    return revealed_count == total_safe;
}

/* Count flagged cells */
static int count_flags(void)
{
    int count = 0;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (display_board[y][x] == FLAGGED) count++;
        }
    }
    return count;
}

/* Display the game board */
static void draw(void)
{
    /* Count flags for display */
    int flag_count = count_flags();
    
    /* Print header */
    printf("Mines: %d  Flags: %d  ", MINES, flag_count);
    if (game_over && won) printf("YOU WIN!");
    else if (game_over) printf("GAME OVER!");
    printf("\n\n");
    
    /* Print column numbers */
    printf("  ");
    for (int x = 0; x < WIDTH; x++) {
        printf("%d ", x % 10);
    }
    printf("\n");
    
    /* Print board */
    for (int y = 0; y < HEIGHT; y++) {
        printf("%d ", y % 10);  /* Row number */
        
        for (int x = 0; x < WIDTH; x++) {
            printf("%c ", display_board[y][x]);
        }
        printf("\n");
    }
    printf("\n");
}

/* Process user input */
static int process_input(void)
{
    char input[32];
    char cmd;
    int x, y;
    
    printf("Command (r row col=Reveal, f row col=Flag, q=Quit): ");
    fflush(stdout);
    
    if (fgets(input, sizeof(input), stdin) == NULL) return 0;
    
    if (sscanf(input, "%c %d %d", &cmd, &x, &y) == 3) {
        if (x < 0 || x >= HEIGHT || y < 0 || y >= WIDTH) {
            printf("Invalid coordinates! Use row 0-%d, col 0-%d\n", HEIGHT-1, WIDTH-1);
            return 1;
        }
        
        if (cmd == 'r' || cmd == 'R') {
            reveal(y, x);
        } else if (cmd == 'f' || cmd == 'F') {
            flag(y, x);
        } else {
            puts("Invalid command! Use 'r' for reveal, 'f' for flag");
            return 1;
        }
    } else if (sscanf(input, "%c", &cmd) == 1) {
        if (cmd == 'q' || cmd == 'Q') {
            return 0;
        } else {
            puts("Invalid command! Use 'r row col' to reveal, 'f row col' to flag, 'q' to quit");
            return 1;
        }
    } else {
        puts("Invalid input! Use 'r row col' to reveal, 'f row col' to flag, 'q' to quit");
        return 1;
    }
    
    return 1;
}

/* Check if game should continue */
static int game_continue(void)
{
    if (game_over) {
        if (won) puts("You won!");
        else puts("You hit a mine!");
        return 0;
    }
    
    won = check_win();
    if (won) {
        game_over = 1;
        return 1;  /* Continue to show win state */
    }
    
    return 1;
}

/* Initialize game board */
static void init_game(void)
{
    /* Clear internal board (hidden state with mines) */
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            board[y][x] = HIDDEN;
            display_board[y][x] = HIDDEN;  /* Player sees all hidden initially */
        }
    }
    
    game_over = 0;
    won = 0;
    first_move = 1;
}

/* Main game loop */
int main(void)
{
    puts("Welcome to Suckless Minesweeper!");
    printf("Board: %dx%d with %d mines\n", WIDTH, HEIGHT, MINES);
    puts("Commands: r row col (reveal), f row col (flag), q (quit)");
    puts("");
    
    srand((unsigned int)time(NULL));
    
    init_game();
    
    while (game_continue()) {
        draw();
        if (!process_input()) break;
    }
    
    draw();
    puts("Thanks for playing!");
    
    return 0;
}
