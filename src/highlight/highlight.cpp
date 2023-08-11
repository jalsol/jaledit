#include "highlight/highlight.hpp"

#include "highlight/lexer.hpp"
#include "highlight/token.hpp"

Highlighter::Highlighter(std::string_view text) : m_lexer{text} {}

HighlightedToken Highlighter::next() {
    Token token = m_lexer.next();
    return HighlightedToken{token, Token::color(token.kind())};
}
