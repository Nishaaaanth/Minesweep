#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#include "raymath.h"

#define COLS 10
#define ROWS 10

const unsigned short screen_width = 400;
const unsigned short screen_height = 400;
const unsigned short cell_width = screen_width/COLS;
const unsigned short cell_height = screen_height/ROWS;

enum RANK {EASY=1, MEDIUM=2, HARD=3};
enum RANK difficulty = EASY;

typedef struct Cell {
    int i;
    int j;
    bool revealed;
    bool contains_mine;
} Cell;
Cell grid[COLS][ROWS];

void cellDraw(Cell);
bool indexIsValid(int, int);
void cellRevealed(int, int);

int main(void) {
    srand(time(0));

    InitWindow(screen_height, screen_width, "Minesweeper");

    for(size_t i=0; i<COLS; ++i) {
        for(size_t j=0; j<ROWS; ++j) {
            grid[i][j] = (Cell) {.i = i, .j = j, .contains_mine=false, .revealed=false};
        }
    }

    int no_of_mines = (int) (COLS * ROWS * difficulty * 0.1f);
    while(no_of_mines>0) {
        int i = rand() % COLS;
        int j = rand() % COLS;
        grid[i][j].contains_mine = true;
        no_of_mines--;
    }

    while(!WindowShouldClose()) {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 m_pos = GetMousePosition();

            int ind_x = m_pos.x / cell_width;
            int ind_y = m_pos.y / cell_height;

            if(indexIsValid(ind_x, ind_y)) {
                cellRevealed(ind_x, ind_y);
            }
        }
        BeginDrawing();

        ClearBackground(RAYWHITE);
        for(size_t i=0; i<COLS; ++i) {
            for(size_t j=0; j<ROWS; ++j) {
                cellDraw(grid[i][j]);
            }
        }

        EndDrawing();
    }

    return 0;
}

void cellDraw(Cell cell) {
    if(cell.revealed) {
        if(cell.contains_mine) {
            DrawRectangle(cell.i * cell_width, cell.j * cell_height, cell_width, cell_height, RED);
        } else {
            DrawRectangle(cell.i * cell_width, cell.j * cell_height, cell_width, cell_height, LIGHTGRAY);
        }
    }
    DrawRectangleLines(cell.i * cell_width, cell.j * cell_height, cell_width, cell_height, BLACK);
}

bool indexIsValid(int i, int j) {
    return (i>=0 && i<COLS && j>=0 && j<ROWS);
}

void cellRevealed(int i, int j) {
    grid[i][j].revealed = true;

    if(grid[i][j].contains_mine) {
        //lose
    } else {
        //game continues
    }
}