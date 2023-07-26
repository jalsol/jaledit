#include "highlight/token.hpp"

#include <string_view>

Token& Token::set_text(std::string_view text) {
    m_text = text;
    return *this;
}

Token& Token::set_kind(TokenKind kind) {
    m_kind = kind;
    return *this;
}

Color Token::color(TokenKind kind) {
    return kind_colors[static_cast<std::size_t>(kind)];
}

std::string_view Token::text() const { return m_text; }

TokenKind Token::kind() const { return m_kind; }

std::string_view Token::kind_name() const {
    return kind_names[static_cast<std::size_t>(m_kind)];
}
