#include "utils.hpp"

#include "constants.hpp"
#include "raylib.h"

#include <cctype>

namespace utils {

void draw_text(const char* text, Vector2 pos, Color color, float font_size,
               float spacing) {
    static Font font = LoadFontEx("data/JetBrainsMonoNerdFont-Regular.ttf",
                                  constants::font_size, nullptr, 0);
    DrawTextEx(font, text, pos, font_size, spacing, color);
}

Vector2 measure_text(const char* text, float font_size, float spacing) {
    static Font font = LoadFontEx("data/JetBrainsMonoNerdFont-Regular.ttf",
                                  constants::font_size, nullptr, 0);
    return MeasureTextEx(font, text, font_size, spacing);
}

int number_len(int n) {
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

bool is_vim_alnum(int c) { return !!std::isalnum(c) || c == '_' || c == '-'; }

} // namespace utils