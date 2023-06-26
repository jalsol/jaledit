#pragma once

#include "piece.h"

#include <stddef.h>

#ifndef PIECE_BUFFER_SIZE
#define PIECE_BUFFER_SIZE ((size_t)(4 * 1024 * 1024))
#endif

typedef struct PieceTable {
    char *original;
    size_t original_size;

    char *add;
    size_t add_size;

    Piece *pieces_head;
    Piece *current_piece;
    int total_lines;
} PieceTable;

PieceTable piece_table_construct(const char *original, size_t original_size);

void piece_table_destruct(PieceTable *piece_table);

void piece_table_add(PieceTable *piece_table, const char *text, size_t text_size);
