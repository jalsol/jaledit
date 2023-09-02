#pragma once

#include "rope/rope.h"

#include "raylib.h"

#include <stdbool.h>
#include <stddef.h>

struct Suggester;
typedef struct Suggester Suggester;

Suggester *suggester_new();
void suggester_delete(Suggester *suggester);

void suggester_load_text(Suggester *suggester, Rope *text);
void suggester_set_pattern(Suggester *suggester, const char *pattern, size_t pattern_len);

void suggester_render(Suggester *suggester, Vector2 origin);
void suggester_set_to_render(Suggester *suggester, bool should_render);
bool suggester_is_rendering(Suggester *suggester);

void suggester_select_next(Suggester *suggester);
void suggester_select_prev(Suggester *suggester);

const char *suggester_get_selected(Suggester *suggester);
void suggester_set_to_update(Suggester *suggester);
bool suggester_should_update(Suggester *suggester);
