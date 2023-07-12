#pragma once

#include <stdlib.h>

#include "constants.hpp"
#include "raylib.h"

namespace utils {

void draw_text(const char* text, Vector2 pos, Color color, float font_size,
               float spacing);

Vector2 measure_text(const char* text, float font_size, float spacing);

int number_len(int n);

bool is_vim_alnum(int c);

} // namespace utils
