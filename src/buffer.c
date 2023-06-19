#include "buffer.h"

#include <stdio.h>
#include <stdlib.h>
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

    while (fgets(line_buffer, sizeof(line_buffer), fp) != NULL) {
        int line_len = strlen(line_buffer);
        while (line_len > 0 && line_buffer[line_len - 1] == '\n') {
            line_buffer[--line_len] = '\0';
        }

        char *line = malloc(line_len + 1);
        memcpy(line, line_buffer, line_len);
        buffer_insert_row(buffer, buffer->rows_len, line, line_len);
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
    memcpy(new_row.content, line, line_len);

    RowNode *new_node = malloc(sizeof(RowNode));
    new_node->row = new_row;
    new_node->next = NULL;
    new_node->prev = NULL;

    if (buffer->rows_len == 0) {
        buffer->rows_head = buffer->rows_tail = new_node;
    } else if (index == 0) {
        new_node->next = buffer->rows_head;
        buffer->rows_head = new_node;
    } else if (index >= buffer->rows_len) {
        new_node->prev = buffer->rows_tail;
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
    return new_node;
}