#include "rope.h"

#include "common/vector.h"
#include "node.h"

struct Rope {
    RopeNode *root;
};

Rope *rope_new() {
    Rope *rope = malloc(sizeof(Rope));
    rope->root = rope_leaf_new("", 0);
    return rope;
}

Rope *rope_new_from_text(const char *text, size_t length) {
    Rope *rope = malloc(sizeof(Rope));
    rope->root = rope_leaf_new(text, length);
    return rope;
}

Rope *rope_clone(Rope *rope) {
    Rope *clone = malloc(sizeof(Rope));
    clone->root = rope_node_copy(rope->root);
    return clone;
}

void rope_delete(Rope *rope) {
    rope_node_delete(rope->root);
    free(rope);
}

size_t rope_length(Rope *rope) { return rope_node_length(rope->root); }

char rope_at(Rope *rope, size_t index) {
    if (index >= rope_length(rope)) {
        return '\0';
    }

    return rope_node_at(rope->root, index);
}

Vec_char *rope_substr(Rope *rope, size_t start, size_t length) {
    Vec_char *result = rope_node_substr(rope->root, start, length);
    vec_char_push_back(result, '\0');
    return result;
}

Rope *rope_insert_text(Rope *rope, size_t index, const char *text, size_t length) {
    Rope *other = rope_new_from_text(text, length);
    Rope *result = rope_insert(rope, index, other);
    rope_delete(other);
    return result;
}

Rope *rope_insert(Rope *rope, size_t index, Rope *other) {
    if (index == 0) {
        return rope_prepend(rope, other);
    }

    if (index >= rope_length(rope)) {
        return rope_append(rope, other);
    }

    RopeSplit split = rope_split(rope, index);
    Rope *tmp = rope_append(split.left, other);
    Rope *result = rope_append(tmp, split.right);

    rope_delete(tmp);
    rope_delete(split.left);
    rope_delete(split.right);

    return result;
}

Rope *rope_append_text(Rope *rope, const char *text, size_t length) {
    Rope *other = rope_new_from_text(text, length);
    Rope *result = rope_append(rope, other);
    rope_delete(other);
    return result;
}

Rope *rope_append(Rope *rope, Rope *other) {
    RopeNode *root =
        rope_branch_new(rope_node_copy(rope->root), rope_node_copy(other->root));

    Rope *result = malloc(sizeof(Rope));
    result->root = root;
    return result;
}

Rope *rope_prepend_text(Rope *rope, const char *text, size_t length) {
    Rope *other = rope_new_from_text(text, length);
    Rope *result = rope_prepend(rope, other);
    rope_delete(other);
    return result;
}

Rope *rope_prepend(Rope *rope, Rope *other) {
    RopeNode *root =
        rope_branch_new(rope_node_copy(other->root), rope_node_copy(rope->root));
    Rope *result = malloc(sizeof(Rope));
    result->root = root;
    return result;
}

Rope *rope_erase(Rope *rope, size_t start, size_t length) {
    RopeSplit split = rope_split(rope, start);

    Rope *result = split.left;
    RopeSplit split2 = rope_split(split.right, length);
    rope_delete(split2.left);

    result = rope_append(result, split2.right);
    rope_delete(split.right);

    return result;
}

Rope *rope_replace_text(Rope *rope, size_t start, size_t length, const char *text,
                        size_t text_length) {
    return rope_replace(rope, start, length, rope_new_from_text(text, text_length));
}

Rope *rope_replace(Rope *rope, size_t start, size_t length, Rope *other) {
    Rope *result = rope_erase(rope, start, length);
    result = rope_insert(result, start, other);
    return result;
}

RopeSplit rope_split(Rope *rope, size_t index) {
    RopeNodeSplit split = rope_node_split(rope->root, index);
    RopeSplit rope_split;

    rope_split.left = malloc(sizeof(Rope));
    rope_split.left->root = split.left;

    rope_split.right = malloc(sizeof(Rope));
    rope_split.right->root = split.right;

    return rope_split;
}

size_t rope_find_line_start(Rope *rope, size_t index) {
    if (index == 0) {
        return 0;
    }

    if (index >= rope_length(rope)) {
        return rope_length(rope);
    }

    return rope_node_find_lf(rope->root, index - 1) + 1;
}

size_t rope_line_cnt(Rope *rope) {
    return rope_node_lf_cnt(rope->root) +
           (rope_node_at(rope->root, rope_length(rope) - 1) != '\n');
}

size_t rope_line_len(Rope *rope, size_t line_index) {
    if (line_index == rope_line_cnt(rope) - 1) {
        return rope_length(rope) - rope_find_line_start(rope, line_index);
    }

    return rope_find_line_start(rope, line_index + 1) -
           rope_find_line_start(rope, line_index) + 1;
}

size_t rope_index_from_pos(Rope *rope, size_t line_index, size_t line_pos) {
    return rope_find_line_start(rope, line_index) + line_pos;
}