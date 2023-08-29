#pragma once

#include "raylib.h"

#include <stdbool.h>
#include <stddef.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) (MAX(MIN((x), (max)), (min)))

bool is_symbol(char c);
bool is_symbol_start(char c);
int num_len(size_t num);

void draw_text(const char *text, Vector2 pos, Color color, float font_size,
               float spacing);

Vector2 measure_text(const char *text, float font_size, float spacing);