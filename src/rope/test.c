#include "rope.h"

#include <stdio.h>

void test1() {
    Rope *rope = rope_new_from_text("Hello\nworld!", 12);
    Vec_char *str = rope_substr(rope, 0, 12);
    printf("data: %s\n", vec_char_data(str));
    vec_char_delete(str);

    printf("len: %zu\n", rope_length(rope));
    printf("line cnt: %zu\n", rope_line_cnt(rope));
    printf("line start: %zu\n", rope_find_line_start(rope, 1));

    Rope *rope2 = rope_replace_text(rope, 0, 5, "Hi", 2);
    size_t len = rope_length(rope2);
    printf("len: %zu\n", len);

    str = rope_substr(rope2, 0, len);
    printf("data2: %s\n", vec_char_data(str));
    vec_char_delete(str);

    rope_delete(rope2);
    rope_delete(rope);
}

void test2() {
    Rope *text = rope_new_from_text("hello world\ncruel hello world\n", 30);
    Rope *tmp1 = rope_replace_text(text, 0, 5, "Hi", 2);

    rope_delete(text);
    text = tmp1;

    tmp1 = rope_replace_text(text, 15, 5, "Hi", 2);
    rope_delete(text);
    text = tmp1;

    Vec_char *str = rope_substr(tmp1, 0, rope_length(text));
    printf("data: %s\n", vec_char_data(str));
    vec_char_delete(str);

    rope_delete(text);
}

void test3() {
    Rope *rope = rope_new_from_text("Hello\nworld!", 12);
    RopeSplit split = rope_split(rope, 5);
    Rope *left = split.left;
    Rope *right = split.right;

    printf("left: %zu\n", rope_length(left));
    printf("right: %zu\n", rope_length(right));

    RopeSplit split2 = rope_split(left, 2);
    Rope *left2 = split2.left;
    Rope *right2 = split2.right;

    printf("left2: %zu\n", rope_length(left2));
    printf("right2: %zu\n", rope_length(right2));

    rope_delete(left2);
    rope_delete(right2);

    rope_delete(left);
    rope_delete(right);
    rope_delete(rope);
}

void test4() {
    Rope *rope = rope_new_from_text("Hello\ncruel\nworld!", 18);

    // erase "cruel\n" step by step manually
    RopeSplit split1 = rope_split(rope, 6);
    RopeSplit split2 = rope_split(split1.right, 6);

    Rope *left = split1.left;
    Rope *right = split2.right;

    rope_delete(split1.right);
    rope_delete(split2.left);

    Rope *tmp = rope_append(left, right);
    rope_delete(left);
    rope_delete(right);

    Vec_char *str = rope_substr(tmp, 0, rope_length(tmp));
    printf("data: %s\n", vec_char_data(str));
    vec_char_delete(str);

    rope_delete(tmp);
    rope_delete(rope);
}

int main() {

    test1();
    test2();
    test3();
    test4();

    return 0;
}