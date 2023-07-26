#pragma once

#include "highlight/lexer.hpp"
#include "highlight/token.hpp"

#include <string_view>

class Highlighter {
public:
    Highlighter(std::string_view text);

    HighlightedToken next();

private:
    Lexer m_lexer;
};