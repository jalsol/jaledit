#include "rope.h"

#include <stdio.h>

int main() {
    Rope *rope = rope_new_from_text("Hello\nworld!", 12);
    Vec_char *str = rope_substr(rope, 0, 12);
    printf("data: %s\n", vec_char_data(str));
    vec_char_delete(str);

    printf("len: %zu\n", rope_length(rope));
    printf("line cnt: %zu\n", rope_line_cnt(rope));
    printf("line start: %zu\n", rope_find_line_start(rope, 1));

    Rope *rope2 = rope_insert_text(rope, 6, "cruel ", 6);
    size_t len = rope_length(rope2);
    printf("len: %zu\n", len);

    str = rope_substr(rope2, 0, len);
    printf("data2: %s\n", vec_char_data(str));
    vec_char_delete(str);

    rope_delete(rope2);
    rope_delete(rope);

    return 0;
}