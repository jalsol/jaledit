#pragma once

#include <stdbool.h>

typedef struct {
    int index;
    int content_len;
    char *content;
} Row;

typedef struct RowNode {
    Row row;
    struct RowNode *next;
    struct RowNode *prev;
} RowNode;

typedef struct {
    int x;
    int y;
    char c;
    RowNode *row;
} Cursor;

typedef struct {
    const char *file_path;

    RowNode *rows_head, *rows_tail;
    int rows_len;

    int offset_x;
    int offset_y;
    Cursor cursor;
    bool dirty;
} Buffer;

Buffer buffer_construct(const char *file_path);
void buffer_destruct(Buffer *buffer);
int buffer_load_file(Buffer *buffer, const char *file_path);
RowNode *buffer_find_row(Buffer *buffer, int index);
RowNode *buffer_insert_row(Buffer *buffer, int index, const char *line,
                           int line_len);
// RowNode *buffer_delete_row(Buffer *buffer, int index);
void buffer_relabel_rows(Buffer *buffer);

void buffer_move_cursor(Buffer *buffer, int dx, int dy);