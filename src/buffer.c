#include "buffer.h"

#include "constants.h"
#include "piece.h"
#include "utils.h"
#include "vector.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

Buffer buffer_construct(const char *file_path) {
    Buffer buffer;
    memset(&buffer, 0, sizeof(Buffer));

    buffer.file_path = file_path;
    if (buffer_load_file(&buffer, file_path) != 0) {
        perror("Could not load file");
        exit(1);
    }

    if (file_path == NULL) {
        buffer.piece_table = piece_table_construct("\n", 1);
    }

    // if the first character of the file is a newline
    // then we set the cursor character to ' '
    // otherwise we set it to the first character of the file
    // note that the first piece is always the original piece

    if (buffer.piece_table.original[0] == '\n') {
        buffer.cursor.c = ' ';
    } else {
        buffer.cursor.c = buffer.piece_table.original[0];
    }

    return buffer;
}

void buffer_destruct(Buffer *buffer) { piece_table_destruct(&buffer->piece_table); }

int buffer_load_file(Buffer *buffer, const char *file_path) {
    if (file_path == NULL) {
        return 0;
    }

    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        return -1;
    }

    char *text = malloc(PIECE_BUFFER_SIZE);
    size_t size = fread(text, sizeof(char), PIECE_BUFFER_SIZE, fp);
    fclose(fp);

    buffer->piece_table = piece_table_construct(text, size);
    free(text);

    return 0;
}

size_t buffer_find_row_start(Buffer *buffer, size_t row_index) {
    Piece *piece = buffer->piece_table.pieces_head;

    for (; piece != NULL; piece = piece->next) {
        if (row_index < piece->line_starts.size) {
            break;
        }

        row_index -= piece->line_starts.size;
    }

    // NOTE: let's assume that the index is always valid
    buffer->piece_table.current_piece = piece;
    return piece->line_starts.data[row_index];
}

void buffer_insert_char(Buffer *buffer, char c) {
    buffer->dirty = true;

    if (c == '\n') {
        piece_table_add(&buffer->piece_table, "\n", 1);
        return;
    }

    char text[2] = {c, '\0'};
    piece_table_add(&buffer->piece_table, text, 1);
}

size_t buffer_dist_until_eol(Buffer *buffer, size_t row_start) {
    Piece *cur_piece = buffer->piece_table.current_piece;
    size_t dist_to_eol = 0;

    for (size_t i = row_start; i < cur_piece->length; ++i) {
        char cur_char;
        if (cur_piece->type == PIECE_ADD) {
            cur_char = buffer->piece_table.add[i + cur_piece->start];
        } else {
            cur_char = buffer->piece_table.original[i + cur_piece->start];
        }

        if (cur_char == '\n') {
            break;
        }

        ++dist_to_eol;
    }

    return dist_to_eol;
}

void buffer_move_cursor(Buffer *buffer, int dx, int dy) {
    int next_x = buffer->cursor.x + dx;
    int next_y = buffer->cursor.y + dy;
    PieceTable *piece_table = &buffer->piece_table;

    if (next_y < 0) {
        next_y = 0;
    } else if (next_y >= piece_table->total_lines) {
        next_y = piece_table->total_lines - 1;
    }

    size_t row_start = buffer_find_row_start(buffer, next_y);
    size_t dist_to_eol = buffer_dist_until_eol(buffer, row_start);

    if (next_x < 0) {
        next_x = 0;
    } else if (next_x >= (int)dist_to_eol) {
        next_x = dist_to_eol - 1;
    }

    char cur_char;
    if (dist_to_eol == 0) {
        next_x = 0;
        cur_char = ' ';
    } else {
        cur_char =
            piece_table->original[row_start + next_x + piece_table->current_piece->start];
    }

    buffer->cursor.x = next_x;
    buffer->cursor.y = next_y;
    buffer->cursor.c = cur_char;
}
