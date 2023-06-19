#include "buffer.h"

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

    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        return -1;
    }

    char line_buffer[1 << 10];
    buffer->rows_len = 0;
    bool has_trailing_newline = false;

    while (fgets(line_buffer, sizeof(line_buffer), fp) != NULL) {
        int line_len = strlen(line_buffer);
        has_trailing_newline = false;

        while (line_len > 0 && line_buffer[line_len - 1] == '\n') {
            line_buffer[--line_len] = '\0';
            has_trailing_newline = true;
        }

        char *line = malloc(line_len + 1);
        memcpy(line, line_buffer, line_len);
        buffer_insert_row(buffer, buffer->rows_len, line, line_len);
    }

    if (has_trailing_newline) {
        buffer_insert_row(buffer, buffer->rows_len, "", 0);
    }

    fclose(fp);
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
    int i = 0;
    for (RowNode *row = buffer->rows_head; row != NULL; row = row->next) {
        row->row.index = ++i;
    }
}