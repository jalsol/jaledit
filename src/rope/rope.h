#pragma once

#include "common/vector.h"

#include <stddef.h>

struct Rope;
typedef struct Rope Rope;

typedef struct RopeSplit {
    Rope *left;
    Rope *right;
} RopeSplit;

Rope *rope_new();
Rope *rope_new_from_text(const char *text, size_t length);
Rope *rope_clone(Rope *rope);
void rope_delete(Rope *rope);

size_t rope_length(Rope *rope);
char rope_at(Rope *rope, size_t index);
Vec_char *rope_substr(Rope *rope, size_t start, size_t length);

// TODO: balance rope

Rope *rope_insert_text(Rope *rope, size_t index, const char *text, size_t length);
Rope *rope_insert(Rope *rope, size_t index, Rope *other);

Rope *rope_append_text(Rope *rope, const char *text, size_t length);
Rope *rope_append(Rope *rope, Rope *other);

Rope *rope_prepend_text(Rope *rope, const char *text, size_t length);
Rope *rope_prepend(Rope *rope, Rope *other);

Rope *rope_erase(Rope *rope, size_t start, size_t length);

Rope *rope_replace_text(Rope *rope, size_t start, size_t length, const char *text,
                        size_t text_length);
Rope *rope_replace(Rope *rope, size_t start, size_t length, Rope *other);

RopeSplit rope_split(Rope *rope, size_t index);

size_t rope_find_line_start(Rope *rope, size_t index);
size_t rope_line_cnt(Rope *rope);
size_t rope_line_len(Rope *rope, size_t line_index);
size_t rope_index_from_pos(Rope *rope, size_t line_index, size_t line_pos);