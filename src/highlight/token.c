#include "token.h"

#include "raylib.h"

const char *token_kind_names[] = {
    "end",         "invalid",    "preproc",     "symbol",      "open_paren",
    "close_paren", "open_curly", "close_curly", "open_square", "close_square",
    "open_attr",   "close_attr", "semicolon",   "keyword",     "comment",
    "string",      "char",       "type",        "number",      "function",
    "operator",
};

static const Color token_kind_colors[] = {
    {0, 0, 0, 0},        // End
    {76, 79, 105, 255},  // Invalid
    {23, 146, 153, 255}, // Preproc
    {76, 79, 105, 255},  // Symbol

    {223, 142, 29, 255}, // OpenParen
    {223, 142, 29, 255}, // CloseParen

    {223, 142, 29, 255}, // OpenCurly
    {223, 142, 29, 255}, // CloseCurly

    {223, 142, 29, 255}, // OpenSquare
    {223, 142, 29, 255}, // CloseSquare

    {223, 142, 29, 255}, // OpenAttr
    {223, 142, 29, 255}, // CloseAttr

    {23, 146, 153, 255}, // Semicolon

    {230, 69, 83, 255},   // Keyword
    {188, 192, 204, 255}, // Comment
    {64, 160, 43, 255},   // String
    {64, 160, 43, 255},   // Char
    {254, 100, 11, 255},  // Type
    {254, 100, 11, 255},  // Number
    {114, 135, 253, 255}, // Function
    {4, 165, 229, 255},   // Operator
};

const char *token_kind_name(TokenKind kind) { return token_kind_names[kind]; }

Color token_kind_color(TokenKind kind) { return token_kind_colors[kind]; }