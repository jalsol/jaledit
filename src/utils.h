#pragma once

#include <stdlib.h>

#include "constants.h"
#include "raylib.h"

static Font *utils_font = NULL;

void draw_text(const char *text, Vector2 pos, Color color, float font_size,
               float spacing);

void measure_text(const char *text, Vector2 pos, Color color, float font_size,
                  float spacing);