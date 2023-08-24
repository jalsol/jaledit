#pragma once

#include "raylib.h"

#include <stddef.h>

typedef enum TokenKind {
    TOKEN_END,
    TOKEN_INVALID,
    TOKEN_PREPROC,
    TOKEN_SYMBOL,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_OPEN_CURLY,
    TOKEN_CLOSE_CURLY,
    TOKEN_OPEN_SQUARE,
    TOKEN_CLOSE_SQUARE,
    TOKEN_OPEN_ATTR,
    TOKEN_CLOSE_ATTR,
    TOKEN_SEMICOLON,
    TOKEN_KEYWORD,
    TOKEN_COMMENT,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_TYPE,
    TOKEN_NUMBER,
    TOKEN_FUNCTION,
    TOKEN_OPERATOR,
} TokenKind;

typedef struct Token {
    TokenKind kind;
    const char *start;
    size_t length;
} Token;

const char *token_kind_name(TokenKind kind);
Color token_kind_color(TokenKind kind);