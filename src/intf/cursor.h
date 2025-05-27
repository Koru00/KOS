#pragma once

typedef enum {
    UP = 0,  // You can start at 0 or any number
    LEFT  = 1,
    DOWN = 2,
    RIGHT  = 3
} Dir;

void update_cursor(int row, int col);
void move_cursor(Dir dir);
