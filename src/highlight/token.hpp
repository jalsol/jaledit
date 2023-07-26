#pragma once

#include "raylib.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

enum class TokenKind : std::size_t {
    End,
    Invalid,
    Preproc,
    Symbol,
    OpenParen,
    CloseParen,
    OpenCurly,
    CloseCurly,
    OpenAttr,
    CloseAttr,
    Semicolon,
    Keyword,
    Comment,
    String,
    Class,
    Number,
};

constexpr std::array<std::string_view, 16> kind_names = {{
    "end",
    "invalid",
    "preproc",
    "symbol",
    "open_paren",
    "close_paren",
    "open_curly",
    "close_curly",
    "open_attr",
    "close_attr",
    "semicolon",
    "keyword",
    "comment",
    "string",
    "class",
    "number",
}};

constexpr std::array<Color, 16> kind_colors = {{
    {0, 0, 0, 0},        // End
    {76, 79, 105, 255},  // Invalid
    {23, 146, 153, 255}, // Preproc
    {76, 79, 105, 255},  // Symbol

    {254, 100, 11, 255}, // OpenParen
    {254, 100, 11, 255}, // CloseParen

    {254, 100, 11, 255}, // OpenCurly
    {254, 100, 11, 255}, // CloseCurly

    {254, 100, 11, 255}, // OpenAttr
    {254, 100, 11, 255}, // CloseAttr

    {23, 146, 153, 255}, // Semicolon

    {230, 69, 83, 255},   // Keyword
    {188, 192, 204, 255}, // Comment
    {64, 160, 43, 255},   // String
    {254, 100, 11, 255},  // Class
    {254, 100, 11, 255},  // Number
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