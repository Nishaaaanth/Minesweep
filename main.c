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
    bool flagged;
    int nearby_mines;
} Cell;
Cell grid[COLS][ROWS];

void cellDraw(Cell);
bool indexIsValid(int, int);
void cellRevealed(int, int);
int countMines(int, int);
void cellFlag(int, int);
void gridInit(void);
void clearCells(int , int);

int main(void) {
    srand(time(0));

    InitWindow(screen_height, screen_width, "Minesweeper");
    gridInit();

    while(!WindowShouldClose()) {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 m_pos = GetMousePosition();

            int ind_x = m_pos.x / cell_width;
            int ind_y = m_pos.y / cell_height;

            if(indexIsValid(ind_x, ind_y)) {
                cellRevealed(ind_x, ind_y);
            }
        } else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 m_pos = GetMousePosition();

            int ind_x = m_pos.x / cell_width;
            int ind_y = m_pos.y / cell_height;

            if(indexIsValid(ind_x, ind_y)) {
                cellFlag(ind_x, ind_y);
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
    char w_offset=cell_width/2;
    char h_offset=cell_height/2;
    if(cell.revealed) {
        if(cell.contains_mine) {
            DrawRectangle(cell.i * cell_width, cell.j * cell_height, cell_width, cell_height, RED);
        } else {
            DrawRectangle(cell.i * cell_width, cell.j * cell_height, cell_width, cell_height, LIGHTGRAY);
            if(cell.nearby_mines > 0)
                DrawText(TextFormat("%d", cell.nearby_mines), (cell.i*cell_width)+w_offset, (cell.j*cell_height)+h_offset, cell_height-h_offset, DARKGRAY);
        }
    }
    else if(cell.flagged) {
        DrawCircle((cell.i * cell_width)+w_offset, (cell.j * cell_height)+h_offset, (int)(cell_width/4), Fade(ORANGE, 0.7f));
    }

    DrawRectangleLines(cell.i * cell_width, cell.j * cell_height, cell_width, cell_height, BLACK);
}

bool indexIsValid(int i, int j) {
    return (i>=0 && i<COLS && j>=0 && j<ROWS);
}

void cellRevealed(int i, int j) {
    if(grid[i][j].flagged) return;

    grid[i][j].revealed = true;

    if(grid[i][j].contains_mine) {
        //lose
    } else {
        //game continues
        if(grid[i][j].nearby_mines == 0)
            clearCells(i , j);
    }
}

int countMines(int i, int j) {
    int count=0;
    for(int i_adj=-1; i_adj<=1; ++i_adj) {
        for(int j_adj=-1; j_adj<=1; ++j_adj) {
            if(i_adj==0 && j_adj==0) continue;
            if(indexIsValid(i+i_adj, j+j_adj) && grid[i+i_adj][j+j_adj].contains_mine) count++;
        }
    }
    return count;
}

void cellFlag(int i, int j) {
    if(grid[i][j].revealed) return;

    grid[i][j].flagged = !grid[i][j].flagged;

}

void gridInit(void) {
    for(size_t i=0; i<COLS; ++i) {
        for(size_t j=0; j<ROWS; ++j) {
            grid[i][j] = (Cell) {
                .i = i,
                .j = j,
                .contains_mine=false,
                .revealed=false,
                .nearby_mines=-1,
                .flagged=false,
            };
        }
    }

    int no_of_mines = (int) (COLS * ROWS * difficulty * 0.1f);
    while(no_of_mines>0) {
        int i = rand() % COLS;
        int j = rand() % COLS;
        
        if(!grid[i][j].contains_mine) {
            grid[i][j].contains_mine = true;
            no_of_mines--;
        }
    }

    for(size_t i=0; i<COLS; ++i) {
        for(size_t j=0; j<ROWS; ++j) {
            if(!grid[i][j].contains_mine) {
                grid[i][j].nearby_mines = countMines(i, j);
            }
        }
    }
}
