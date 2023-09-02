#include "lexer.h"

#include "common/utils.h"
#include "token.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct LiteralToken {
    const char *text;
    TokenKind kind;
} LiteralToken;

static const LiteralToken literal_tokens[] = {
    {"(", TOKEN_OPEN_PAREN},  {")", TOKEN_CLOSE_PAREN}, {"{", TOKEN_OPEN_CURLY},
    {"}", TOKEN_CLOSE_CURLY}, {"[", TOKEN_OPEN_SQUARE}, {"]", TOKEN_CLOSE_SQUARE},
    {";", TOKEN_SEMICOLON},   {"[[", TOKEN_OPEN_ATTR},  {"]]", TOKEN_CLOSE_ATTR},
    {"->", TOKEN_OPERATOR},   {"::", TOKEN_OPERATOR},   {"<<", TOKEN_OPERATOR},
    {">>", TOKEN_OPERATOR},   {"+=", TOKEN_OPERATOR},   {"-=", TOKEN_OPERATOR},
    {"*=", TOKEN_OPERATOR},   {"/=", TOKEN_OPERATOR},   {"%=", TOKEN_OPERATOR},
    {"<=", TOKEN_OPERATOR},   {">=", TOKEN_OPERATOR},   {"!=", TOKEN_OPERATOR},
    {"&=", TOKEN_OPERATOR},   {"~=", TOKEN_OPERATOR},   {"^=", TOKEN_OPERATOR},
    {"==", TOKEN_OPERATOR},   {".", TOKEN_OPERATOR},    {",", TOKEN_OPERATOR},
    {"?", TOKEN_OPERATOR},    {":", TOKEN_OPERATOR},    {"+", TOKEN_OPERATOR},
    {"-", TOKEN_OPERATOR},    {"*", TOKEN_OPERATOR},    {"/", TOKEN_OPERATOR},
    {"%", TOKEN_OPERATOR},    {"<", TOKEN_OPERATOR},    {">", TOKEN_OPERATOR},
    {"!", TOKEN_OPERATOR},    {"&", TOKEN_OPERATOR},    {"~", TOKEN_OPERATOR},
    {"^", TOKEN_OPERATOR},    {"=", TOKEN_OPERATOR},
};

#define LITERAL_TOKEN_COUNT (sizeof(literal_tokens) / sizeof(LiteralToken))

static const char *keywords[] = {
    "auto",
    "break",
    "case",
    "const",
    "continue",
    "default",
    "do",
    "else",
    "enum",
    "extern",
    "for",
    "goto",
    "if",
    "register",
    "return",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "volatile",
    "while",
    "alignas",
    "alignof",
    "and",
    "and_eq",
    "asm",
    "atomic_cancel",
    "atomic_commit",
    "atomic_noexcept",
    "bitand",
    "bitor",
    "catch",
    "class",
    "co_await",
    "co_return",
    "co_yield",
    "compl",
    "concept",
    "const_cast",
    "consteval",
    "constexpr",
    "constinit",
    "decltype",
    "delete",
    "dynamic_cast",
    "explicit",
    "export",
    "false",
    "friend",
    "inline",
    "mutable",
    "namespace",
    "new",
    "nodiscard",
    "noexcept",
    "not",
    "not_eq",
    "nullptr",
    "operator",
    "or",
    "or_eq",
    "private",
    "protected",
    "public",
    "reflexpr",
    "reinterpret_cast",
    "requires",
    "static_assert",
    "static_cast",
    "synchronized",
    "template",
    "this",
    "thread_local",
    "throw",
    "true",
    "try",
    "typeid",
    "typename",
    "using",
    "virtual",
    "xor",
    "xor_eq",
};

#define KEYWORD_COUNT (sizeof(keywords) / sizeof(char *))

static const char *types[] = {
    "bool",     "char", "float",    "double",   "int",     "long",    "short",  "signed",
    "unsigned", "void", "char16_t", "char32_t", "char8_t", "wchar_t", "size_t",
};

#define TYPE_COUNT (sizeof(types) / sizeof(char *))

struct Lexer {
    const char *text;
};

Lexer *lexer_new(const char *text) {
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->text = text;
    return lexer;
}

void lexer_delete(Lexer *lexer) { free(lexer); }

void lexer_skip(Lexer *lexer, size_t n) { lexer->text += n; }

void lexer_trim_left(Lexer *lexer) {
    while (isspace(*lexer->text)) {
        lexer_skip(lexer, 1);
    }
}

bool lexer_starts_with(Lexer *lexer, const char *str) {
    return strncmp(lexer->text, str, strlen(str)) == 0;
}

Token lexer_next(Lexer *lexer) {
    Token token = {0};
    token.kind = TOKEN_END;
    token.start = lexer->text;

    if (*lexer->text == '\0') {
        return token;
    }

    if (*lexer->text == '"') {
        token.kind = TOKEN_STRING;
        lexer_skip(lexer, 1);

        while (*lexer->text != '\0' && *lexer->text != '"') {
            lexer_skip(lexer, 1);
        }

        if (*lexer->text != '\0') {
            lexer_skip(lexer, 1);
        }

        token.length = lexer->text - token.start;
        return token;
    }

    if (*lexer->text == '\'') {
        token.kind = TOKEN_CHAR;
        lexer_skip(lexer, 1);

        while (*lexer->text != '\0' && *lexer->text != '\'') {
            lexer_skip(lexer, 1);
        }

        if (*lexer->text != '\0') {
            lexer_skip(lexer, 1);
        }

        token.length = lexer->text - token.start;
        return token;
    }

    if (*lexer->text == '#') {
        token.kind = TOKEN_PREPROC;

        while (*lexer->text != '\0' && *lexer->text != '\n') {
            lexer_skip(lexer, 1);
        }
        if (*lexer->text != '\0') {
            lexer_skip(lexer, 1);
        }

        token.length = lexer->text - token.start;
        return token;
    }

    if (lexer_starts_with(lexer, "//")) {
        token.kind = TOKEN_COMMENT;

        while (*lexer->text != '\0' && *lexer->text != '\n') {
            lexer_skip(lexer, 1);
        }
        if (*lexer->text != '\0') {
            lexer_skip(lexer, 1);
        }

        token.length = lexer->text - token.start;
        return token;
    }

    for (size_t i = 0; i < LITERAL_TOKEN_COUNT; i++) {
        if (lexer_starts_with(lexer, literal_tokens[i].text)) {
            token.kind = literal_tokens[i].kind;
            lexer_skip(lexer, strlen(literal_tokens[i].text));
            token.length = lexer->text - token.start;
            return token;
        }
    }

    if (is_symbol_start(*lexer->text)) {
        token.kind = TOKEN_SYMBOL;

        while (is_symbol(*lexer->text)) {
            lexer_skip(lexer, 1);
        }

        token.length = lexer->text - token.start;

        if (toupper(*token.start) == *token.start) {
            token.kind = TOKEN_TYPE;
        }

        for (size_t i = 0; i < TYPE_COUNT; i++) {
            if (strncmp(token.start, types[i], token.length) == 0) {
                token.kind = TOKEN_TYPE;
                break;
            }
        }

        for (size_t i = 0; i < KEYWORD_COUNT; i++) {
            if (strncmp(token.start, keywords[i], token.length) == 0) {
                token.kind = TOKEN_KEYWORD;
                break;
            }
        }

        if (*lexer->text == '(') {
            token.kind = TOKEN_FUNCTION;
        }

        return token;
    } else if (isdigit(*lexer->text)) {
        token.kind = TOKEN_NUMBER;

        while (isdigit(*lexer->text)) {
            lexer_skip(lexer, 1);
        }

        token.length = lexer->text - token.start;
        return token;
    }

    lexer_skip(lexer, 1);
    token.kind = TOKEN_INVALID;
    token.length = lexer->text - token.start;
    return token;
}
