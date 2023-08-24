#include "finder.h"

#include "common/vector.h"

#include <stdio.h>

int main(void) {
    Finder *finder = finder_new();

    Rope *text = rope_new_from_text("hello world\ncruel hello world\n", 30);

    finder_append_char(finder, 'h');
    finder_append_char(finder, 'e');
    finder_append_char(finder, 'l');
    finder_append_char(finder, 'l');
    finder_append_char(finder, 'o');

    finder_find_in_content(finder, text);

    printf("matches: %zu\n", vec_Cursor_size(finder_matches(finder)));
    Cursor cursor = {0, 0};

    for (size_t i = 0; i < vec_Cursor_size(finder_matches(finder)); i++) {
        cursor = vec_Cursor_at(finder_matches(finder), i);
        printf("match %zu: %d:%d\n", i, cursor.line, cursor.column);
    }

    finder_toggle_prompt(finder, FINDER_REPLACE);
    finder_switch_active_input(finder);

    finder_append_char(finder, 'b');
    finder_append_char(finder, 'y');
    finder_append_char(finder, 'e');

    Rope *replaced = finder_replace_in_content(finder, text);
    Vec_char *replaced_text = rope_substr(replaced, 0, rope_length(replaced));
    vec_char_push_back(replaced_text, '\0');
    printf("replaced: %s\n", vec_char_data(replaced_text));
    vec_char_delete(replaced_text);
    rope_delete(replaced);

    finder_delete(finder);
    rope_delete(text);
}