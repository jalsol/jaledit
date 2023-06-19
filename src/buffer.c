#include "buffer.h"

#include "constants.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

Buffer buffer_construct(const char *file_path) {
    Buffer buffer;
    memset(&buffer, 0, sizeof(Buffer));

    buffer.file_path = file_path;
    if (buffer_load_file(&buffer, file_path) == -1) {
        perror("Can't open file");
        exit(1);
    }

    if (buffer.rows_len == 0) {
        buffer_insert_row(&buffer, 0, "", 0);
    }

    buffer.cursor.row = buffer.rows_head;

    if (buffer.rows_head->row.content_len > 0) {
        buffer.cursor.c = buffer.rows_head->row.content[0];
    } else {
        buffer.cursor.c = ' ';
    }

    return buffer;
}

void buffer_destruct(Buffer *buffer) {
    for (RowNode *row = buffer->rows_head; row != NULL;) {
        RowNode *next = row->next;
        free(row->row.content);
        free(row);
        row = next;
    }
}

int buffer_load_file(Buffer *buffer, const char *file_path) {
    if (file_path == NULL) {
        return 0;
    }

    FILE *file_ptr = fopen(file_path, "r");
    if (file_ptr == NULL) {
        return -1;
    }

    char line_buffer[BUFFER_LIMIT];
    buffer->rows_len = 0;
    bool has_trailing_newline = false;

    while (fgets(line_buffer, sizeof(line_buffer), file_ptr) != NULL) {
        size_t line_len = strlen(line_buffer);
        has_trailing_newline = false;

        while (line_len > 0 && line_buffer[line_len - 1] == '\n') {
            line_buffer[--line_len] = '\0';
            has_trailing_newline = true;
        }

        char *line = malloc(line_len + 1);
        memcpy(line, line_buffer, line_len);
        buffer_insert_row(buffer, buffer->rows_len, line, line_len);
        free(line);
    }

    if (has_trailing_newline) {
        buffer_insert_row(buffer, buffer->rows_len, "", 0);
    }

    fclose(file_ptr);
    return 0;
}

RowNode *buffer_find_row(Buffer *buffer, int index) {
    RowNode *ptr = buffer->rows_head;

    for (int i = 0; i < index && ptr != buffer->rows_tail; ++i) {
        ptr = ptr->next;
    }

    return ptr;
}

RowNode *buffer_insert_row(Buffer *buffer, int index, const char *line,
                           int line_len) {
    Row new_row;
    new_row.content_len = line_len;
    new_row.content = malloc(line_len + 1);
    new_row.content[line_len] = '\0';
    memcpy(new_row.content, line, line_len);

    RowNode *new_node = malloc(sizeof(RowNode));
    new_node->row = new_row;
    new_node->next = NULL;
    new_node->prev = NULL;

    if (buffer->rows_len == 0) {
        buffer->rows_head = buffer->rows_tail = new_node;
    } else if (index == 0) {
        new_node->next = buffer->rows_head;
        buffer->rows_head->prev = new_node;
        buffer->rows_head = new_node;
    } else if (index >= buffer->rows_len) {
        new_node->prev = buffer->rows_tail;
        buffer->rows_tail->next = new_node;
        buffer->rows_tail = new_node;
    } else {
        RowNode *right_pivot = buffer_find_row(buffer, index);
        RowNode *left_pivot = right_pivot->prev;

        right_pivot->prev = new_node;
        left_pivot->next = new_node;

        new_node->next = right_pivot;
        new_node->prev = left_pivot;
    }

    ++buffer->rows_len;
    buffer_relabel_rows(buffer);
    return new_node;
}

void buffer_relabel_rows(Buffer *buffer) {
    int idx = 0;
    for (RowNode *row = buffer->rows_head; row != NULL; row = row->next) {
        row->row.index = ++idx;
    }
}

void buffer_move_cursor(Buffer *buffer, int dx, int dy) {
    int next_x = buffer->cursor.x + dx;
    int next_y = buffer->cursor.y + dy;

    if (next_y < 0) {
        next_y = 0;
    } else if (next_y >= buffer->rows_len) {
        next_y = buffer->rows_len - 1;
    }

    if (next_y > buffer->cursor.y) {
        for (int i = buffer->cursor.y; i < next_y; ++i) {
            buffer->cursor.row = buffer->cursor.row->next;
        }
    } else if (next_y < buffer->cursor.y) {
        for (int i = buffer->cursor.y; i > next_y; --i) {
            buffer->cursor.row = buffer->cursor.row->prev;
        }
    }

    if (next_x < 0) {
        next_x = 0;
    } else if (next_x >= buffer->cursor.row->row.content_len) {
        next_x = buffer->cursor.row->row.content_len - 1;
    }

    if (buffer->cursor.row->row.content_len == 0) {
        next_x = 0;
    }

    buffer->cursor.x = next_x;
    buffer->cursor.y = next_y;
    buffer->cursor.c = buffer->cursor.row->row.content[buffer->cursor.x];
}
