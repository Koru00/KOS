#pragma once

typedef enum {
    UP,
    LEFT,
    DOWN,
    RIGHT
} Dir;

void init_cursor();
void update_cursor(int row, int col);
void move_cursor(Dir dir);
