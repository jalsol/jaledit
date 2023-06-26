#pragma once

#include "vector.h"

typedef enum PieceType {
    PIECE_ORIGINAL,
    PIECE_ADD,
} PieceType;

typedef struct Piece {
    PieceType type;
    size_t start;
    size_t length;
    Vector line_starts;
    struct Piece *next;
} Piece;

Piece *piece_construct(PieceType type, const char *str, int start, int length);
void piece_destruct(Piece *piece);