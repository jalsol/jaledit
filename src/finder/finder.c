#include "finder.h"

#include "common/utils.h"
#include "common/vector.h"
#include "rope/rope.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct Finder {
    Vec_char *pattern;
    Vec_char *replacement;
    Vec_Cursor *matches;
    Vec_size_t *match_idx;

    FinderMode mode;
    bool active_rope; // 0: pattern, 1: replacement
    bool should_highlight;
};

Finder *finder_new() {
    Finder *finder = calloc(1, sizeof(Finder));
    finder->pattern = vec_char_new();
    finder->replacement = vec_char_new();
    finder->matches = vec_Cursor_new();
    finder->match_idx = vec_size_t_new();
    finder->mode = FINDER_NONE;
    finder->active_rope = 0;
    finder->should_highlight = false;
    return finder;
}

void finder_delete(Finder *finder) {
    vec_char_delete(finder->pattern);
    vec_char_delete(finder->replacement);
    vec_Cursor_delete(finder->matches);
    vec_size_t_delete(finder->match_idx);
    free(finder);
}

void finder_find_in_content(Finder *finder, Rope *rope) {
    vec_Cursor_clear(finder->matches);
    vec_size_t_clear(finder->match_idx);

    if (vec_char_size(finder->pattern) == 0) {
        return;
    }

    Rope *tmp1 = rope_new_from_text(vec_char_data(finder->pattern),
                                    vec_char_size(finder->pattern));
    Rope *tmp2 = rope_append_text(tmp1, "$", 1);
    Rope *z_text = rope_append(tmp2, rope);
    rope_delete(tmp1);
    rope_delete(tmp2);

    size_t z_len = rope_length(z_text);
    size_t z[z_len];
    memset(z, 0, z_len * sizeof(size_t));
    size_t l = 0;
    size_t r = 0;

    for (size_t i = 1; i < z_len; i++) {
        if (i < r) {
            z[i] = MIN(r - i, z[i - l]);
        }
        while (i + z[i] < z_len && rope_at(z_text, i + z[i]) == rope_at(z_text, z[i])) {
            z[i]++;
        }
        if (i + z[i] > r) {
            l = i;
            r = i + z[i];
        }
    }

    Cursor cursor = {0, 0};
    size_t pat_len = vec_char_size(finder->pattern);
    for (size_t i = pat_len + 1; i < z_len; i++) {
        char c = rope_at(z_text, i);

        if (z[i] == pat_len) {
            vec_Cursor_push_back(finder->matches, cursor);
            vec_size_t_push_back(finder->match_idx, i - pat_len - 1);
        }

        if (c == '\n') {
            cursor.line++;
            cursor.column = 0;
        } else {
            cursor.column++;
        }
    }

    rope_delete(z_text);
}

Rope *finder_replace_in_content(Finder *finder, Rope *rope) {
    finder_find_in_content(finder, rope);
    Rope *result = rope_clone(rope);

    size_t match_idx_len = vec_size_t_size(finder->match_idx);
    if (match_idx_len == 0) {
        return result;
    }

    size_t pat_len = vec_char_size(finder->pattern);
    char *replacement = vec_char_data(finder->replacement);
    size_t replacement_len = vec_char_size(finder->replacement);

    for (size_t i = 1; i-- > 0;) {
        size_t idx = vec_size_t_at(finder->match_idx, i);
        Rope *tmp = rope_replace_text(result, idx, pat_len, replacement, replacement_len);
        rope_delete(result);
        result = tmp;
    }

    return result;
}

Cursor finder_next_match(Finder *finder, Cursor cursor) {
    if (vec_Cursor_size(finder->matches) == 0) {
        return cursor;
    }

    // find upper bound with binary search
    size_t l = 0;
    size_t r = vec_Cursor_size(finder->matches);
    while (l < r) {
        size_t m = l + (r - l) / 2;
        Cursor c = vec_Cursor_at(finder->matches, m);
        if (cursor_cmp(cursor, c) < 0) {
            r = m;
        } else {
            l = m + 1;
        }
    }

    if (l == vec_Cursor_size(finder->matches)) {
        l = 0;
    }

    return vec_Cursor_at(finder->matches, l);
}

Cursor finder_prev_match(Finder *finder, Cursor cursor) {
    if (vec_Cursor_size(finder->matches) == 0) {
        return cursor;
    }

    // find lower bound with binary search
    size_t l = 0;
    size_t r = vec_Cursor_size(finder->matches);
    while (l < r) {
        size_t m = l + (r - l) / 2;
        Cursor c = vec_Cursor_at(finder->matches, m);
        if (cursor_cmp(cursor, c) <= 0) {
            r = m;
        } else {
            l = m + 1;
        }
    }

    if (l == 0) {
        l = vec_Cursor_size(finder->matches);
    }

    return vec_Cursor_at(finder->matches, l - 1);
}

Vec_Cursor *finder_matches(Finder *finder) { return finder->matches; }

bool finder_is_active(Finder *finder) { return finder->mode != FINDER_NONE; }

FinderMode finder_mode(Finder *finder) { return finder->mode; }

void finder_toggle_prompt(Finder *finder, FinderMode mode) {
    if (finder->mode == mode) {
        finder->mode = FINDER_NONE;
    } else {
        finder->mode = mode;
    }
}

void finder_switch_active_input(Finder *finder) {
    if (finder->mode == FINDER_NONE) {
        return;
    }

    finder->active_rope = !finder->active_rope;
}

void finder_append_char(Finder *finder, char c) {
    if (finder->active_rope == 0) {
        vec_char_push_back(finder->pattern, c);
    } else {
        vec_char_push_back(finder->replacement, c);
    }
}

void finder_pop_char(Finder *finder) {
    if (finder->active_rope == 0) {
        if (vec_char_size(finder->pattern) == 0) {
            return;
        }

        vec_char_pop_back(finder->pattern);
    } else {
        if (vec_char_size(finder->replacement) == 0) {
            return;
        }

        vec_char_pop_back(finder->replacement);
    }
}

Vec_char *finder_pattern(Finder *finder) { return finder->pattern; }

Vec_char *finder_replacement(Finder *finder) { return finder->replacement; }

void finder_set_to_highlight(Finder *finder, bool should_highlight) {
    finder->should_highlight = should_highlight;
}

bool finder_should_highlight(Finder *finder) { return finder->should_highlight; }