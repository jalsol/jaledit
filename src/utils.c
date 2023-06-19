#include "utils.h"

#include <stdlib.h>

#include "constants.h"
#include "raylib.h"

void draw_text(const char *text, Vector2 pos, Color color, float font_size,
               float spacing) {
    if (utils_font == NULL) {
        utils_font = malloc(sizeof(Font));
        *utils_font = LoadFontEx("data/JetBrainsMonoNerdFont-Regular.ttf",
                                 FONT_SIZE, NULL, 0);
    }

    DrawTextEx(*utils_font, text, pos, font_size, spacing, color);
}

Vector2 measure_text(const char *text, float font_size, float spacing) {
    if (utils_font == NULL) {
        utils_font = malloc(sizeof(Font));
        *utils_font = LoadFontEx("data/JetBrainsMonoNerdFont-Regular.ttf",
                                 FONT_SIZE, NULL, 0);
    }
    return MeasureTextEx(*utils_font, text, font_size, spacing);
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