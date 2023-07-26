#include "highlight/lexer.hpp"

#include "highlight/token.hpp"

#include <array>
#include <cctype>
#include <string_view>

constexpr std::array<LiteralToken, 41> literal_tokens = {{
    {"(", TokenKind::OpenParen},  {")", TokenKind::CloseParen},
    {"{", TokenKind::OpenCurly},  {"}", TokenKind::CloseCurly},
    {"[", TokenKind::OpenSquare}, {"]", TokenKind::CloseSquare},
    {";", TokenKind::Semicolon},  {"[[", TokenKind::OpenAttr},
    {"]]", TokenKind::CloseAttr}, {"->", TokenKind::Operator},
    {"::", TokenKind::Operator},  {"<<", TokenKind::Operator},
    {">>", TokenKind::Operator},  {"+=", TokenKind::Operator},
    {"-=", TokenKind::Operator},  {"*=", TokenKind::Operator},
    {"/=", TokenKind::Operator},  {"%=", TokenKind::Operator},
    {"<=", TokenKind::Operator},  {">=", TokenKind::Operator},
    {"!=", TokenKind::Operator},  {"&=", TokenKind::Operator},
    {"~=", TokenKind::Operator},  {"^=", TokenKind::Operator},
    {"==", TokenKind::Operator},  {".", TokenKind::Operator},
    {",", TokenKind::Operator},   {"?", TokenKind::Operator},
    {":", TokenKind::Operator},   {"+", TokenKind::Operator},
    {"-", TokenKind::Operator},   {"*", TokenKind::Operator},
    {"/", TokenKind::Operator},   {"%", TokenKind::Operator},
    {"<", TokenKind::Operator},   {">", TokenKind::Operator},
    {"!", TokenKind::Operator},   {"&", TokenKind::Operator},
    {"~", TokenKind::Operator},   {"^", TokenKind::Operator},
    {"=", TokenKind::Operator},
}};

constexpr std::array<std::string_view, 14> types = {{
    "bool",
    "char",
    "float",
    "double",
    "int",
    "long",
    "short",
    "signed",
    "unsigned",
    "void",
    "char16_t",
    "char32_t",
    "char8_t",
    "wchar_t",
}};

constexpr std::array<std::string_view, 84> keywords = {{
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
}};

Lexer::Lexer(std::string_view text) : m_text{text} {};

bool Lexer::starts_with(std::string_view prefix) const {
    return m_text.substr(m_pos).starts_with(prefix);
}

void Lexer::skip(std::size_t n) { m_pos += n; }

void Lexer::trim_left() {
    while (m_pos < m_text.size() && std::isspace(m_text[m_pos])) {
        skip(1);
    }
}

bool is_symbol_start(char x) { return std::isalpha(x) || x == '_'; }

bool is_symbol(char x) { return std::isalnum(x) || x == '_'; }

Token Lexer::next() {
    Token token;
    token.set_text(m_text.substr(m_pos)).set_kind(TokenKind::End);
    std::size_t old_pos = m_pos;

    if (m_pos >= m_text.size()) {
        return token;
    }

    if (m_text[m_pos] == '"') {
        token.set_kind(TokenKind::String);
        skip(1);

        while (m_pos < m_text.size() && m_text[m_pos] != '"'
               && m_text[m_pos] != '\n') {
            skip(1);
        }

        if (m_pos < m_text.size()) {
            skip(1);
        }

        token.set_text(m_text.substr(old_pos, m_pos - old_pos));
        return token;
    }

    if (m_text[m_pos] == '\'') {
        token.set_kind(TokenKind::Char);
        skip(1);

        while (m_pos < m_text.size() && m_text[m_pos] != '\''
               && m_text[m_pos] != '\n') {
            skip(1);
        }

        if (m_pos < m_text.size()) {
            skip(1);
        }

        token.set_text(m_text.substr(old_pos, m_pos - old_pos));
        return token;
    }

    if (m_text[m_pos] == '#') {
        token.set_kind(TokenKind::Preproc);
        while (m_pos < m_text.size() && m_text[m_pos] != '\n') {
            skip(1);
        }
        if (m_pos < m_text.size()) {
            skip(1);
        }

        token.set_text(m_text.substr(old_pos, m_pos - old_pos));
        return token;
    }

    if (starts_with("//")) {
        token.set_kind(TokenKind::Comment);

        while (m_pos < m_text.size() && m_text[m_pos] != '\n') {
            skip(1);
        }
        if (m_pos < m_text.size()) {
            skip(1);
        }

        token.set_text(m_text.substr(old_pos, m_pos - old_pos));
        return token;
    }

    for (const auto& [text, token_kind] : literal_tokens) {
        if (starts_with(text)) {
            token.set_kind(token_kind);
            token.set_text(text);
            skip(text.size());
            return token;
        }
    }

    if (is_symbol_start(m_text[m_pos])) {
        token.set_kind(TokenKind::Symbol);

        while (m_pos < m_text.size() && is_symbol(m_text[m_pos])) {
            skip(1);
        }

        auto len = m_pos - old_pos;
        auto symbol = m_text.substr(old_pos, len);
        token.set_text(symbol);

        if (std::toupper(m_text[old_pos]) == m_text[old_pos]) {
            token.set_kind(TokenKind::Type);
        }

        for (auto type : types) {
            if (len == type.size() && type == symbol) {
                token.set_kind(TokenKind::Type);
                break;
            }
        }

        for (auto keyword : keywords) {
            if (len == keyword.size() && keyword == symbol) {
                token.set_kind(TokenKind::Keyword);
                break;
            }
        }

        if (m_pos + 1 < m_text.size() && m_text[m_pos] == '(') {
            token.set_kind(TokenKind::Function);
        }

        return token;
    } else if (std::isdigit(m_text[m_pos])) {
        token.set_kind(TokenKind::Number);

        while (m_pos < m_text.size() && is_symbol(m_text[m_pos])) {
            skip(1);
        }

        auto len = m_pos - old_pos;
        auto symbol = m_text.substr(old_pos, len);
        token.set_text(symbol);

        return token;
    }

    skip(1);
    token.set_kind(TokenKind::Invalid).set_text(m_text.substr(old_pos, 1));
    return token;
}