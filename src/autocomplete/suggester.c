#include "suggester.h"

#include "common/utils.h"
#include "common/vector.h"
#include "rope/rope.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define MAX_SUGGESTIONS 10

typedef struct ScoredMatch {
    size_t kw_index;
    int score;
} ScoredMatch;

struct Suggester {
    Rope *text;
    const char *pattern;
    bool rendering;
    bool should_update;
    size_t selected;
    Vec_ScoredMatchPtr *matches;
    Vec_Vec_charPtr *keywords;
};

Suggester *Suggester_new() {
    Suggester *suggester = malloc(sizeof(Suggester));
    suggester->text = NULL;
    suggester->pattern = NULL;
    suggester->rendering = false;
    suggester->should_update = true;
    suggester->selected = 0;
    suggester->matches = vec_ScoredMatchPtr_new();
    suggester->keywords = vec_Vec_charPtr_new();
    return suggester;
}

void suggester_delete(Suggester *suggester) {
    for (size_t i = 0; i < vec_Vec_charPtr_size(suggester->keywords); i++) {
        vec_char_delete(vec_Vec_charPtr_at(suggester->keywords, i));
    }
    vec_Vec_charPtr_delete(suggester->keywords);

    for (size_t i = 0; i < vec_ScoredMatchPtr_size(suggester->matches); i++) {
        free(vec_ScoredMatchPtr_at(suggester->matches, i));
    }
    vec_ScoredMatchPtr_delete(suggester->matches);

    free(suggester);
}

void suggester_load_text(Suggester *suggester, Rope *text) {
    suggester->text = text;
    vec_Vec_charPtr_clear(suggester->keywords);

    for (size_t line = 0; line < rope_line_cnt(text); line++) {
        size_t line_start = rope_find_line_start(text, line);
        size_t line_len = rope_line_len(text, line);
        Vec_char *line_text = rope_substr(text, line_start, line_len);
        vec_char_push_back(line_text, '\0');

        for (size_t i = 0; i < vec_char_size(line_text); i++) {
            if (!is_symbol(vec_char_at(line_text, i))) {
                continue;
            }

            size_t j = i + 1;
            while (j < vec_char_size(line_text) && is_symbol(vec_char_at(line_text, j))) {
                j++;
            }

            Vec_char *keyword = vec_char_new();
            for (size_t k = i; k < j; k++) {
                vec_char_push_back(keyword, vec_char_at(line_text, k));
            }
            vec_char_push_back(keyword, '\0');
            vec_Vec_charPtr_push_back(suggester->keywords, keyword);

            i = j;
        }

        vec_char_delete(line_text);
    }

    // remove duplicates from keywords
    size_t kw_size = vec_Vec_charPtr_size(suggester->keywords);
    for (size_t i = 0; i < kw_size; i++) {
        for (size_t j = i + 1; j < kw_size; j++) {
            Vec_char *kw_i = vec_Vec_charPtr_at(suggester->keywords, i);
            Vec_char *kw_j = vec_Vec_charPtr_at(suggester->keywords, j);
            if (strcmp(vec_char_data(kw_i), vec_char_data(kw_j)) == 0) {
                vec_char_delete(kw_j);
                vec_Vec_charPtr_erase(suggester->keywords, j);
                j--;
                kw_size--;
            }
        }
    }

    suggester->should_update = false;
}

int calc_score(const char *keyword, const char *pattern) {
    const int match = 1;
    const int mismatch = -8;
    const int gap = -4;

    size_t kw_len = strlen(keyword);
    size_t pat_len = strlen(pattern);
    int max_score = 0;

    int matrix[kw_len + 1][pat_len + 1];
    memset(matrix, 0, sizeof(matrix));

    for (size_t i = 1; i <= kw_len; i++) {
        for (size_t j = 1; j <= pat_len; j++) {
            const int score = (keyword[i - 1] == pattern[j - 1]) ? match : mismatch;

            matrix[i][j] = MAX(MAX(matrix[i - 1][j - 1] + score, 0),
                               MAX(matrix[i - 1][j] + gap, matrix[i][j - 1] + gap));

            max_score = MAX(max_score, matrix[i][j]);
        }
    }

    return max_score;
}

void suggester_set_pattern(Suggester *suggester, const char *pattern,
                           size_t pattern_len) {
    suggester->pattern = pattern;
    vec_ScoredMatchPtr_clear(suggester->matches);

    if (pattern_len == 0) {
        return;
    }

    for (size_t i = 0; i < vec_Vec_charPtr_size(suggester->keywords); ++i) {
        Vec_char *keyword = vec_Vec_charPtr_at(suggester->keywords, i);
        int score = calc_score(vec_char_data(keyword), pattern);

        if (score > 0) {
            ScoredMatch *match = malloc(sizeof(ScoredMatch));
            match->kw_index = i;
            match->score = score;
            vec_ScoredMatchPtr_push_back(suggester->matches, match);
        }
    }

    // sort matches by maximum score
    size_t size = vec_ScoredMatchPtr_size(suggester->matches);
    for (size_t i = 0; i < size; i++) {
        for (size_t j = i + 1; j < size; j++) {
            ScoredMatch *match_i = vec_ScoredMatchPtr_at(suggester->matches, i);
            ScoredMatch *match_j = vec_ScoredMatchPtr_at(suggester->matches, j);
            if (match_i->score < match_j->score) {
                vec_ScoredMatchPtr_set(suggester->matches, i, match_j);
                vec_ScoredMatchPtr_set(suggester->matches, j, match_i);
            }
        }
    }

    printf("matches:\n");
    for (size_t i = 0; i < vec_ScoredMatchPtr_size(suggester->matches); i++) {
        ScoredMatch *match = vec_ScoredMatchPtr_at(suggester->matches, i);
        printf("%s: %d\n",
               vec_char_data(vec_Vec_charPtr_at(suggester->keywords, match->kw_index)),
               match->score);
    }
}

void suggester_set_to_render(Suggester *suggester) { suggester->rendering = true; }

bool suggester_is_rendering(Suggester *suggester) { return suggester->rendering; }

void suggester_select_next(Suggester *suggester) {
    if (vec_ScoredMatchPtr_size(suggester->matches) == 0) {
        return;
    }

    if (!suggester->rendering) {
        suggester->rendering = true;
        return;
    }

    size_t rendered_items =
        MIN(vec_ScoredMatchPtr_size(suggester->matches), MAX_SUGGESTIONS);
    suggester->selected = (suggester->selected + 1) % rendered_items;
}

void suggester_select_prev(Suggester *suggester) {
    if (vec_ScoredMatchPtr_size(suggester->matches) == 0) {
        return;
    }

    if (!suggester->rendering) {
        suggester->rendering = true;
        return;
    }

    size_t rendered_items =
        MIN(vec_ScoredMatchPtr_size(suggester->matches), MAX_SUGGESTIONS);
    suggester->selected = (suggester->selected + rendered_items - 1) % rendered_items;
}

const char *suggester_get_selected(Suggester *suggester) {
    if (vec_ScoredMatchPtr_size(suggester->matches) == 0) {
        return NULL;
    }

    ScoredMatch *match = vec_ScoredMatchPtr_at(suggester->matches, suggester->selected);
    return vec_char_data(vec_Vec_charPtr_at(suggester->keywords, match->kw_index));
}

void suggester_set_to_update(Suggester *suggester) { suggester->should_update = true; }

bool suggester_should_update(Suggester *suggester) { return suggester->should_update; }