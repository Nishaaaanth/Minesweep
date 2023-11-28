#include <stdlib.h>
#include <time.h>
#include "raylib.h"

#define COLS 10
#define ROWS 10

const unsigned short screen_width = 400;
const unsigned short screen_height = 400;
const unsigned short cell_width = screen_width/COLS;
const unsigned short cell_height = screen_height/ROWS;
const char *game_lost = "YOU LOST!";
const char *game_won = "YOU WON!";
const char *restart = "Press \'SPACE\' to restart!"; 
const char result_size = 26;
unsigned short cells_revealed = 0;
unsigned short mines_available = 0;
unsigned short start_time;
unsigned short end_time;

enum RANK {EASY=1, MEDIUM=2, HARD=3};
enum RANK difficulty = EASY;

typedef enum GameState {
    PLAYING,
    LOSE,
    WIN
} GameState;
GameState state = PLAYING;

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
void reset();

int main(void) {
    srand(time(0));

    InitWindow(screen_height, screen_width, "Minesweeper");
    reset();

    while(!WindowShouldClose()) {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 m_pos = GetMousePosition();

            int ind_x = m_pos.x / cell_width;
            int ind_y = m_pos.y / cell_height;

            if(indexIsValid(ind_x, ind_y) && state == PLAYING) {
                cellRevealed(ind_x, ind_y);
            }
        } else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 m_pos = GetMousePosition();

            int ind_x = m_pos.x / cell_width;
            int ind_y = m_pos.y / cell_height;

            if(indexIsValid(ind_x, ind_y) && state == PLAYING) {
                cellFlag(ind_x, ind_y);
            }
        }

        if(IsKeyPressed(KEY_SPACE))
            reset();

        BeginDrawing();

        ClearBackground(RAYWHITE);
        for(size_t i=0; i<COLS; ++i) {
            for(size_t j=0; j<ROWS; ++j) {
                cellDraw(grid[i][j]);
            }
        }

        if (state == LOSE) {
            DrawRectangle(0, 0, screen_width, screen_height, Fade(WHITE, 0.8f));

            DrawText(game_lost, (screen_width-MeasureText(game_lost, result_size))/2, screen_height/2-(result_size/2), result_size, DARKGRAY);
            DrawText(restart, (screen_width-MeasureText(restart, result_size*0.8))/2, screen_height*0.6f, result_size*0.8, DARKGRAY);

            DrawText(TextFormat("Time Lapsed: %d min, %d sec", end_time/60, end_time%60), (result_size*0.7f)/2, (result_size*0.7f)/2, result_size*0.7f, DARKGRAY);
        }

        if (state == WIN) {
            DrawRectangle(0, 0, screen_width, screen_height, Fade(WHITE, 0.8f));

            DrawText(game_won, (screen_width-MeasureText(game_won, result_size))/2, screen_height/2-(result_size/2), result_size, DARKGRAY);
            DrawText(restart, (screen_width-MeasureText(restart, result_size*0.8))/2, screen_height*0.6f, result_size*0.8, DARKGRAY);
            DrawText(TextFormat("Time Lapsed: %d min, %d sec", end_time/60, end_time%60), (result_size*0.7f)/2, (result_size*0.7f)/2, result_size*0.7f, DARKGRAY);
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
            DrawCircle((cell.i * cell_width)+cell_width/2, (cell.j * cell_height)+cell_height/2, (int)cell_width/4, RED);
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
        state = LOSE;
        end_time = GetTime();
    } else {
        if(grid[i][j].nearby_mines == 0) {
            clearCells(i , j);
        }
        cells_revealed++;

        if(cells_revealed == ROWS * COLS - mines_available) {
            state = WIN;
            end_time = GetTime();
        }
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
    
    mines_available = (int) (COLS * ROWS * difficulty * 0.1f);
    int no_of_mines = mines_available;
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

void clearCells(int i, int j) {
    for(int i_adj=-1; i_adj<=1; ++i_adj) {
        for(int j_adj=-1; j_adj<=1; ++j_adj) {
            if(i_adj==0 && j_adj==0) continue;
            if(indexIsValid(i+i_adj, j+j_adj) && !grid[i+i_adj][j+j_adj].revealed) cellRevealed(i+i_adj, j+j_adj);
        }
    }
}

void reset(void) {
    gridInit();
    state = PLAYING;
    start_time = GetTime();
}
