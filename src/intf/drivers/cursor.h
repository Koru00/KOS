#pragma once

typedef enum {
    UP,
    LEFT,
    DOWN,
    RIGHT
} Dir;

typedef enum {
  BLOCK,
  UNDERLINE,
  HALF,
  INVISIBLE
} Shape;

void init_cursor();
void update_cursor(int row, int col);
void move_cursor(Dir dir);
void cursor_shape(Shape shape);
