#pragma once

#include <stdlib.h>

#include "constants.h"
#include "raylib.h"

static Font *utils_font = NULL;

void draw_text(const char *text, Vector2 pos, Color color, float font_size,
               float spacing);

Vector2 measure_text(const char *text, float font_size, float spacing);

int number_len(int n);