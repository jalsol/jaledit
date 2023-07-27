#pragma once

#include <cstdlib>

#include "constants.hpp"
#include "raylib.h"

namespace utils {

void draw_text(const char* text, Vector2 pos, Color color, float font_size,
               float spacing);

Vector2 measure_text(const char* text, float font_size, float spacing);

template<typename Tp>
int number_len(Tp n) {
    if (n == 0) {
        return 1;
    }

    int len = 0;
    do {
        len++;
        n /= 10;
    } while (n > 0);
    return len;
}

bool is_vim_alnum(int c);

void unreachable();

bool is_symbol_start(char x);

bool is_symbol(char x);

} // namespace utils
