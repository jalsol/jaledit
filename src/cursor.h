#pragma once

typedef struct Cursor {
    int line;
    int column;
} Cursor;

int cursor_cmp(Cursor a, Cursor b);