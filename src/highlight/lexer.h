#pragma once

#include "token.h"

struct Lexer;
typedef struct Lexer Lexer;

Lexer *lexer_new(const char *text);
void lexer_delete(Lexer *lexer);

Token lexer_next(Lexer *lexer);
void lexer_trim_left(Lexer *lexer);