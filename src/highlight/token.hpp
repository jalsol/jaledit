#pragma once

#include "raylib.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace constants {

constexpr std::size_t token_count = 21;

} // namespace constants

enum class TokenKind : std::size_t {
    End,
    Invalid,
    Preproc,
    Symbol,
    OpenParen,
    CloseParen,
    OpenCurly,
    CloseCurly,
    OpenSquare,
    CloseSquare,
    OpenAttr,
    CloseAttr,
    Semicolon,
    Keyword,
    Comment,
    String,
    Char,
    Type,
    Number,
    Function,
    Operator,
};

constexpr std::array<std::string_view, constants::token_count> kind_names = {{
    "end",
    "invalid",
    "preproc",
    "symbol",
    "open_paren",
    "close_paren",
    "open_curly",
    "close_curly",
    "open_square",
    "close_square"
    "open_attr",
    "close_attr",
    "semicolon",
    "keyword",
    "comment",
    "string",
    "char",
    "type",
    "number",
    "function",
    "operator",
}};

constexpr std::array<Color, constants::token_count> kind_colors = {{
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
}};

class Token {
public:
    std::string_view text() const;
    TokenKind kind() const;
    std::string_view kind_name() const;

    Token& set_text(std::string_view text);
    Token& set_kind(TokenKind kind);

    static Color color(TokenKind kind);

private:
    TokenKind m_kind{};
    std::string m_text{};
};

struct LiteralToken {
    std::string_view text;
    TokenKind kind;
};

struct HighlightedToken {
    Token token;
    Color color;
};