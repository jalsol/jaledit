#pragma once

#include "common/vector.h"
#include "cursor.h"
#include "rope/rope.h"

#include <stdbool.h>

typedef enum FinderMode {
    FINDER_NONE,
    FINDER_FIND,
    FINDER_REPLACE,
} FinderMode;

struct Finder;
typedef struct Finder Finder;

Finder *finder_new();
void finder_delete(Finder *finder);

void finder_find_in_content(Finder *finder, Rope *rope);
Rope *finder_replace_in_content(Finder *finder, Rope *rope);

Cursor finder_next_match(Finder *finder, Cursor cursor);
Cursor finder_prev_match(Finder *finder, Cursor cursor);

Vec_Cursor *finder_matches(Finder *finder);
// Vec_size_t *finder_match_idx(Finder *finder);

bool finder_is_active(Finder *finder);
void finder_render(Finder *finder);

void finder_toggle_prompt(Finder *finder, FinderMode mode);
FinderMode finder_mode(Finder *finder);

void finder_switch_active_input(Finder *finder);

void finder_append_char(Finder *finder, char c);
void finder_pop_char(Finder *finder);

Vec_char *finder_pattern(Finder *finder);
Vec_char *finder_replacement(Finder *finder);

void finder_set_to_highlight(Finder *finder, bool should_highlight);
bool finder_should_highlight(Finder *finder);