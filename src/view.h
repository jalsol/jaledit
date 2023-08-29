#pragma once

#include "cursor.h"
#include "raylib.h"

#include <stdbool.h>

typedef struct View {
    int offset_line;
    int offset_column;
    int header_size;
} View;

bool view_is_cursor_viewable(View view, Cursor cursor, Vector2 char_size);

bool view_is_line_viewable(View view, int line, Vector2 char_size);
int viewable_lines_cnt(Vector2 char_size);

bool view_is_column_viewable(View view, int column, Vector2 char_size);
int viewable_columns_cnt(View view, Vector2 char_size);