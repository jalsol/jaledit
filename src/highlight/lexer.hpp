#pragma once

#include "highlight/token.hpp"

#include <cstddef>
#include <string_view>

class Lexer {
public:
    Lexer(std::string_view text);
    Token next();

private:
    std::string_view m_text;
    std::size_t m_pos{};

    bool starts_with(std::string_view prefix) const;
    void skip(std::size_t n);
    void trim_left();
};