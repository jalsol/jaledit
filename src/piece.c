#include "piece.h"

#include <stdlib.h>

Piece *piece_construct(PieceType type, const char *str, int start, int length) {
    Piece *piece = malloc(sizeof(Piece));
    piece->type = type;
    piece->start = start;
    piece->length = length;
    piece->next = NULL;

    piece->line_starts = vector_construct();
    vector_push_back(&piece->line_starts, 0); // line 0 starts at char 0

    for (int i = 0; i < length; i++) {
        if (str[i] == '\n') {
            vector_push_back(&piece->line_starts, i + 1);
        }
    }

    return piece;
}

void piece_destruct(Piece *piece) {
    piece->start = 0;
    piece->length = 0;
    piece->next = NULL;
    vector_destruct(&piece->line_starts);
    free(piece);
}