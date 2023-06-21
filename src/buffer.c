#include "buffer.h"

#include "constants.h"
#include "utils.h"

#include <ctype.h>
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

    buffer.cursor.row_node = buffer.row_node_head;

    if (buffer.row_node_head->row.content_len > 0) {
        buffer.cursor.c = buffer.row_node_head->row.content[0];
    } else {
        buffer.cursor.c = ' ';
    }

    return buffer;
}

void buffer_destruct(Buffer *buffer) {
    for (RowNode *row_node = buffer->row_node_head; row_node != NULL;) {
        RowNode *next = row_node->next;
        free(row_node->row.content);
        free(row_node);
        row_node = next;
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
    RowNode *ptr = buffer->row_node_head;

    for (int i = 0; i < index && ptr != buffer->row_node_tail; ++i) {
        ptr = ptr->next;
    }

    return ptr;
}

RowNode *buffer_insert_row(Buffer *buffer, int index, const char *line, int line_len) {
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
        buffer->row_node_head = buffer->row_node_tail = new_node;
    } else if (index == 0) {
        new_node->next = buffer->row_node_head;
        buffer->row_node_head->prev = new_node;
        buffer->row_node_head = new_node;
    } else if (index >= buffer->rows_len) {
        new_node->prev = buffer->row_node_tail;
        buffer->row_node_tail->next = new_node;
        buffer->row_node_tail = new_node;
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
    for (RowNode *row_node = buffer->row_node_head; row_node != NULL;
         row_node = row_node->next) {
        row_node->row.index = ++idx;
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
            buffer->cursor.row_node = buffer->cursor.row_node->next;
        }
    } else if (next_y < buffer->cursor.y) {
        for (int i = buffer->cursor.y; i > next_y; --i) {
            buffer->cursor.row_node = buffer->cursor.row_node->prev;
        }
    }

    if (next_x < 0) {
        next_x = 0;
    } else if (next_x >= buffer->cursor.row_node->row.content_len) {
        next_x = buffer->cursor.row_node->row.content_len - 1;
    }

    if (buffer->cursor.row_node->row.content_len == 0) {
        next_x = 0;
    }

    buffer->cursor.x = next_x;
    buffer->cursor.y = next_y;
    buffer->cursor.c = buffer->cursor.row_node->row.content[buffer->cursor.x];
}

int buffer_move_to_next_char(Buffer *buffer) {
    int *x = &buffer->cursor.x;
    int *y = &buffer->cursor.y;
    RowNode **row_node = &buffer->cursor.row_node;
    int ret_val = 0;

    if (*x < (*row_node)->row.content_len - 1) {
        ++*x;
    } else if (*y < buffer->rows_len - 1) {
        ++*y;
        *x = 0;
        *row_node = (*row_node)->next;
    } else {
        *y = buffer->rows_len - 1;
        *x = (*row_node)->row.content_len - 1;
        *row_node = buffer->row_node_tail;
        ret_val = -1;
    }

    if ((*row_node)->row.content_len == 0) {
        buffer->cursor.c = ' ';
    } else {
        buffer->cursor.c = (*row_node)->row.content[*x];
    }

    return ret_val;
}

int buffer_move_to_prev_char(Buffer *buffer) {
    int *x = &buffer->cursor.x;
    int *y = &buffer->cursor.y;
    RowNode **row_node = &buffer->cursor.row_node;
    int ret_val = 0;

    if (*x > 0) {
        --*x;
    } else if (*y > 0) {
        --*y;
        *row_node = (*row_node)->prev;
        *x = (*row_node)->row.content_len - 1;
    } else {
        *y = 0;
        *x = 0;
        *row_node = buffer->row_node_head;
        ret_val = -1;
    }

    if ((*row_node)->row.content_len == 0) {
        buffer->cursor.c = ' ';
    } else {
        buffer->cursor.c = (*row_node)->row.content[*x];
    }

    return ret_val;
}

void buffer_move_to_next_word(Buffer *buffer) {
    int *x = &buffer->cursor.x;
    int *y = &buffer->cursor.y;
    RowNode **row_node = &buffer->cursor.row_node;

    bool alnum_word = !!isalnum(buffer->cursor.c);
    bool punct_word = !!ispunct(buffer->cursor.c);

    // if already in word, move to end of word
    if (alnum_word) {
        while (isalnum(buffer->cursor.c)) {
            buffer_move_to_next_char(buffer);
        }
    } else if (punct_word) {
        buffer_move_to_next_char(buffer);
        if (ispunct(buffer->cursor.c)) {
            return;
        }
    }

    if (isblank(buffer->cursor.c)) {
        while (isblank(buffer->cursor.c)) {
            buffer_move_to_next_char(buffer);
        }
    }
}

void buffer_move_to_prev_word(Buffer *buffer) {
    int *x = &buffer->cursor.x;
    int *y = &buffer->cursor.y;
    RowNode **row_node = &buffer->cursor.row_node;

    buffer_move_to_prev_char(buffer);

    while (isblank(buffer->cursor.c)) {
        buffer_move_to_prev_char(buffer);
    }

    bool alnum_word = !!isalnum(buffer_peak_prev_char(buffer));
    bool punct_word = !!ispunct(buffer_peak_prev_char(buffer));

    // if already in word, move to beginning of word
    if (alnum_word) {
        while (isalnum(buffer_peak_prev_char(buffer))) {
            buffer_move_to_prev_char(buffer);
        }
    } else if (punct_word) {
        buffer_move_to_prev_char(buffer);
        if (ispunct(buffer_peak_prev_char(buffer))) {
            return;
        }
    }
}

char buffer_peak_prev_char(Buffer *buffer) {
    int x = buffer->cursor.x;
    int y = buffer->cursor.y;
    RowNode *row_node = buffer->cursor.row_node;

    if (x > 0) {
        return row_node->row.content[x - 1];
    } else if (y > 0) {
        return row_node->prev->row.content[row_node->prev->row.content_len - 1];
    } else {
        return ' ';
    }
}