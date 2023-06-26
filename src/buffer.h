#pragma once

#include "piece_table.h"

#include <stdbool.h>

typedef struct {
    int x;
    int y;
    char c;
} Cursor;

typedef struct Buffer {
    const char *file_path;

    PieceTable piece_table;
    Cursor cursor;
    bool dirty;

    int view_offset_col;
    int view_offset_row;
} Buffer;

Buffer buffer_construct(const char *file_path);
void buffer_destruct(Buffer *buffer);

int buffer_load_file(Buffer *buffer, const char *file_path);

size_t buffer_dist_until_eol(Buffer *buffer, size_t row_start);
size_t buffer_find_row_start(Buffer *buffer, size_t row_index);

void buffer_insert_char(Buffer *buffer, char c);

void buffer_move_cursor(Buffer *buffer, int dx, int dy);
// void buffer_move_cursor_to_next_char(Buffer *buffer);
// void buffer_move_cursor_to_prev_char(Buffer *buffer);
// char buffer_peak_prev_char(Buffer *buffer);

// void buffer_move_cursor_to_next_word(Buffer *buffer);
// void buffer_move_cursor_to_prev_word(Buffer *buffer);

// int buffer_load_file(Buffer *buffer, const char *file_path);
// RowNode *buffer_find_row(Buffer *buffer, int index);
// RowNode *buffer_insert_row(Buffer *buffer, int index, const char *line, int line_len);
// RowNode *buffer_delete_row(Buffer *buffer, int index);
// void buffer_relabel_rows(Buffer *buffer);

// char buffer_peak_prev_char(Buffer *buffer);

// int buffer_move_to_next_char(Buffer *buffer);
// int buffer_move_to_prev_char(Buffer *buffer);
// void buffer_move_to_next_word(Buffer *buffer);
// void buffer_move_to_prev_word(Buffer *buffer);