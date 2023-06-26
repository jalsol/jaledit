#include "piece_table.h"

#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PieceTable piece_table_construct(const char *original, size_t original_size) {
    PieceTable piece_table = {
        .original_size = original_size,
        .add_size = 0,
    };

    piece_table.add = malloc(PIECE_BUFFER_SIZE);

    if (original != NULL) {
        piece_table.original = calloc(original_size, sizeof(char));
        memcpy(piece_table.original, original, original_size);
    }

    Piece *piece = piece_construct(PIECE_ORIGINAL, original, 0, original_size);
    piece_table.pieces_head = piece;
    piece_table.current_piece = piece;
    piece_table.total_lines = piece->line_starts.size;

    return piece_table;
}

void piece_table_destruct(PieceTable *piece_table) {
    Piece *piece = piece_table->pieces_head;
    while (piece) {
        Piece *next = piece->next;
        piece_destruct(piece);
        piece = next;
    }

    free(piece_table->original);
    free(piece_table->add);

    piece_table->pieces_head = NULL;
    piece_table->current_piece = NULL;
    piece_table->original = NULL;
    piece_table->add = NULL;
}

void piece_table_add(PieceTable *piece_table, const char *text, size_t text_size) {
    if (piece_table->add_size + text_size > PIECE_BUFFER_SIZE) {
        fprintf(stderr, "Piece buffer overflow\n");
        exit(1);
    }

    memcpy(piece_table->add + piece_table->add_size, text, text_size);
    piece_table->add_size += text_size;

    Piece *piece =
        piece_construct(PIECE_ADD, text, piece_table->add_size - text_size, text_size);
    piece_table->pieces_head = piece;
}