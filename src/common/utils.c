#include "utils.h"
#include "constants.h"
#include "raylib.h"

#include <ctype.h>
#include <stdlib.h>

Font *utils_font = NULL;

void draw_text(const char *text, Vector2 pos, Color color, float font_size,
               float spacing) {
    if (utils_font == NULL) {
        utils_font = malloc(sizeof(Font));
        *utils_font =
            LoadFontEx("data/JetBrainsMonoNerdFont-Regular.ttf", FONT_SIZE, NULL, 0);
    }

    DrawTextEx(*utils_font, text, pos, font_size, spacing, color);
}

Vector2 measure_text(const char *text, float font_size, float spacing) {
    if (utils_font == NULL) {
        utils_font = malloc(sizeof(Font));
        *utils_font =
            LoadFontEx("data/JetBrainsMonoNerdFont-Regular.ttf", FONT_SIZE, NULL, 0);
    }
    return MeasureTextEx(*utils_font, text, font_size, spacing);
}

bool is_symbol_start(char c) { return isalpha(c) || c == '_'; }

bool is_symbol(char c) { return isalnum(c) || c == '_'; }

int num_len(size_t num) {
    if (num == 0) {
        return 1;
    }

    int result = 0;

    do {
        ++result;
        num /= 10;
    } while (num > 0);

    return result;
}