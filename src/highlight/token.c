#include "token.h"

#include "raylib.h"

const char *token_kind_names[] = {
    "end",         "invalid",    "preproc",     "symbol",      "open_paren",
    "close_paren", "open_curly", "close_curly", "open_square", "close_square",
    "open_attr",   "close_attr", "semicolon",   "keyword",     "comment",
    "string",      "char",       "type",        "number",      "function",
    "operator",
};

Color token_kind_colors[] = {
    (Color){0, 0, 0, 0},        // End
    (Color){76, 79, 105, 255},  // Invalid
    (Color){23, 146, 153, 255}, // Preproc
    (Color){76, 79, 105, 255},  // Symbol

    (Color){223, 142, 29, 255}, // OpenParen
    (Color){223, 142, 29, 255}, // CloseParen

    (Color){223, 142, 29, 255}, // OpenCurly
    (Color){223, 142, 29, 255}, // CloseCurly

    (Color){223, 142, 29, 255}, // OpenSquare
    (Color){223, 142, 29, 255}, // CloseSquare

    (Color){223, 142, 29, 255}, // OpenAttr
    (Color){223, 142, 29, 255}, // CloseAttr

    (Color){23, 146, 153, 255}, // Semicolon

    (Color){230, 69, 83, 255},   // Keyword
    (Color){188, 192, 204, 255}, // Comment
    (Color){64, 160, 43, 255},   // String
    (Color){64, 160, 43, 255},   // Char
    (Color){254, 100, 11, 255},  // Type
    (Color){254, 100, 11, 255},  // Number
    (Color){114, 135, 253, 255}, // Function
    (Color){4, 165, 229, 255},   // Operator
};

const char *token_kind_name(TokenKind kind) { return token_kind_names[kind]; }

Color token_kind_color(TokenKind kind) { return token_kind_colors[kind]; }