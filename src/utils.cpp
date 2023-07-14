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

bool is_vim_alnum(int c) { return !!std::isalnum(c) || c == '_' || c == '-'; }

} // namespace utils