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

void measure_text(const char *text, Vector2 pos, Color color, float font_size,
                  float spacing) {
    if (utils_font == NULL) {
        utils_font = malloc(sizeof(Font));
        *utils_font = LoadFontEx("data/JetBrainsMonoNerdFont-Regular.ttf",
                                 FONT_SIZE, NULL, 0);
    }
    MeasureTextEx(*utils_font, text, font_size, spacing);
}