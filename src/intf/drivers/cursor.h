#pragma once

typedef enum {
    UP = 0,
    LEFT  = 1,
    DOWN = 2,
    RIGHT  = 3
} Dir;

void update_cursor(int row, int col);
void move_cursor(Dir dir);
