#include "view.h"

#include "common/constants.h"
#include "raylib.h"

bool view_is_cursor_viewable(View view, Cursor cursor, Vector2 char_size) {
    return view_is_line_viewable(view, cursor.line, char_size) &&
           view_is_column_viewable(view, cursor.column, char_size);
}

bool view_is_line_viewable(View view, int line, Vector2 char_size) {
    return line >= view.offset_line &&
           line < view.offset_line + viewable_lines_cnt(char_size);
}

int viewable_lines_cnt(Vector2 char_size) {
    return (GetScreenHeight() - MARGIN) / (char_size.y + LINE_SPACING);
}

bool view_is_column_viewable(View view, int column, Vector2 char_size) {
    return column >= view.offset_column &&
           column < view.offset_column + viewable_columns_cnt(view, char_size);
}

int viewable_columns_cnt(View view, Vector2 char_size) {
    return (GetScreenWidth() - MARGIN - 1) / char_size.x - view.header_size - 1;
}