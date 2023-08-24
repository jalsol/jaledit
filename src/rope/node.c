#include "node.h"

#include "common/utils.h"
#include "common/vector.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

struct RopeNode {
    enum RopeNodeKind kind;

    size_t depth;
    size_t length;
    size_t weight;

    // lf = line feed
    size_t lf_count;
    size_t lf_weight;

    union {
        struct {
            Vec_char *data;
            Vec_size_t *lf_pos; // lf = line feed
        } leaf;
        struct {
            RopeNode *left;
            RopeNode *right;
        } branch;
    };

    size_t *ref_count;
};

RopeNode *rope_leaf_new(const char *text, size_t length) {
    RopeNode *node = malloc(sizeof(RopeNode));
    node->kind = ROPE_NODE_LEAF;

    node->depth = 0;
    node->length = length;
    node->weight = length;

    node->leaf.data = vec_char_new();
    vec_char_resize(node->leaf.data, length);
    memcpy(vec_char_data(node->leaf.data), text, length);

    node->leaf.lf_pos = vec_size_t_new();
    for (size_t i = 0; i < length; i++) {
        if (text[i] == '\n') {
            vec_size_t_push_back(node->leaf.lf_pos, i);
        }
    }

    node->lf_count = vec_size_t_size(node->leaf.lf_pos);
    node->lf_weight = node->lf_count;

    node->ref_count = malloc(sizeof(size_t));
    *node->ref_count = 1;

    return node;
}

void rope_leaf_delete(RopeNode *node) {
    if (*node->ref_count > 1) {
        (*node->ref_count)--;
        return;
    }

    vec_char_delete(node->leaf.data);
    vec_size_t_delete(node->leaf.lf_pos);
    free(node->ref_count);
    free(node);
}

char rope_leaf_at(RopeNode *node, size_t index) {
    return vec_char_at(node->leaf.data, index);
}

Vec_char *rope_leaf_substr(RopeNode *node, size_t start, size_t length) {
    return vec_char_new_subvec(node->leaf.data, start, start + length);
}

RopeNodeSplit rope_leaf_split(RopeNode *node, size_t index) {
    RopeNodeSplit split;
    split.left = rope_leaf_new(vec_char_data(node->leaf.data), index);
    split.right =
        rope_leaf_new(vec_char_data(node->leaf.data) + index, node->length - index);

    return split;
}

// Vec_RopeNodePtr *rope_leaf_leaves(RopeNode *node) {
//     Vec_RopeNodePtr *leaves = vec_RopeNodePtr_new();
//     vec_RopeNodePtr_push_back(leaves, node);
//     return leaves;
// }

size_t rope_leaf_find_lf(RopeNode *node, size_t index) {
    return vec_size_t_at(node->leaf.lf_pos, index);
}

RopeNode *rope_branch_new(RopeNode *left, RopeNode *right) {
    RopeNode *node = malloc(sizeof(RopeNode));
    node->kind = ROPE_NODE_BRANCH;

    node->branch.left = left;
    node->branch.right = right;

    node->weight = left ? left->length : 0;
    node->length = node->weight + (right ? right->length : 0);

    node->lf_weight = left ? left->lf_count : 0;
    node->lf_count = node->lf_weight + (right ? right->lf_count : 0);

    node->depth = MAX(left ? left->depth : 0, right ? right->depth : 0) + 1;

    node->ref_count = malloc(sizeof(size_t));
    *node->ref_count = 1;

    return node;
}

void rope_branch_delete(RopeNode *node) {
    if (*node->ref_count > 1) {
        (*node->ref_count)--;
        return;
    }

    rope_node_delete(node->branch.left);
    rope_node_delete(node->branch.right);
    free(node->ref_count);
    free(node);
}

char rope_branch_at(RopeNode *node, size_t index) {
    if (index < node->weight) {
        return rope_node_at(node->branch.left, index);
    } else {
        return rope_node_at(node->branch.right, index - node->weight);
    }
}

Vec_char *rope_branch_substr(RopeNode *node, size_t start, size_t length) {
    if (start >= node->weight) {
        if (node->branch.right) {
            return rope_node_substr(node->branch.right, start - node->weight, length);
        } else {
            return vec_char_new();
        }
    }

    Vec_char *result = vec_char_new();
    vec_char_reserve(result, length);

    if (node->branch.left) {
        Vec_char *left = rope_node_substr(node->branch.left, start, length);
        for (size_t i = 0; i < vec_char_size(left); i++) {
            vec_char_push_back(result, vec_char_at(left, i));
        }
        vec_char_delete(left);
    }

    if (start + length > node->weight) {
        if (node->branch.right) {
            size_t offset = node->weight - start;
            Vec_char *right = rope_node_substr(node->branch.right, 0, length - offset);
            for (size_t i = 0; i < vec_char_size(right); i++) {
                vec_char_push_back(result, vec_char_at(right, i));
            }
            vec_char_delete(right);
        }
    }

    return result;
}

RopeNodeSplit rope_branch_split(RopeNode *node, size_t index) {
    if (index == node->weight) {
        return (RopeNodeSplit){rope_node_copy(node->branch.left),
                               rope_node_copy(node->branch.right)};
    }

    if (index < node->weight) {
        RopeNodeSplit lsplit = {NULL, NULL};
        if (node->branch.left) {
            lsplit = rope_node_split(node->branch.left, index);
        }

        return (RopeNodeSplit){rope_node_copy(lsplit.left),
                               rope_branch_new(lsplit.right, node->branch.right)};
    } else {
        RopeNodeSplit rsplit = {NULL, NULL};
        if (node->branch.right) {
            rsplit = rope_node_split(node->branch.right, index - node->weight);
        }

        return (RopeNodeSplit){rope_branch_new(node->branch.left, rsplit.left),
                               rope_node_copy(rsplit.right)};
    }
}

// Vec_RopeNodePtr *rope_branch_leaves(RopeNode *node) {
//     Vec_RopeNodePtr *result = vec_RopeNodePtr_new();

//     if (node->branch.left) {
//         Vec_RopeNodePtr *left = rope_node_leaves(node->branch.left);
//         for (size_t i = 0; i < vec_RopeNodePtr_size(left); i++) {
//             vec_RopeNodePtr_push_back(result, vec_RopeNodePtr_at(left, i));
//         }
//         vec_RopeNodePtr_delete(left);
//     }
//     if (node->branch.right) {
//         Vec_RopeNodePtr *right = rope_node_leaves(node->branch.right);
//         for (size_t i = 0; i < vec_RopeNodePtr_size(right); i++) {
//             vec_RopeNodePtr_push_back(result, vec_RopeNodePtr_at(right, i));
//         }
//         vec_RopeNodePtr_delete(right);
//     }

//     return result;
// }

size_t rope_branch_find_lf(RopeNode *node, size_t index) {
    if (index < node->lf_count) {
        return rope_node_find_lf(node->branch.left, index);
    } else {
        return node->weight +
               rope_node_find_lf(node->branch.right, index - node->lf_count);
    }
}

char rope_node_at(RopeNode *node, size_t index) {
    switch (node->kind) {
    case ROPE_NODE_LEAF:
        return rope_leaf_at(node, index);
    case ROPE_NODE_BRANCH:
        return rope_branch_at(node, index);
    }
    assert(false);
}

RopeNodeSplit rope_node_split(RopeNode *node, size_t index) {
    switch (node->kind) {
    case ROPE_NODE_LEAF:
        return rope_leaf_split(node, index);
    case ROPE_NODE_BRANCH:
        return rope_branch_split(node, index);
    }
    assert(false);
}

size_t rope_node_find_lf(RopeNode *node, size_t index) {
    switch (node->kind) {
    case ROPE_NODE_LEAF:
        return rope_leaf_find_lf(node, index);
    case ROPE_NODE_BRANCH:
        return rope_branch_find_lf(node, index);
    }
    assert(false);
}

void rope_node_delete(RopeNode *node) {
    if (!node) {
        return;
    }

    switch (node->kind) {
    case ROPE_NODE_LEAF:
        rope_leaf_delete(node);
        break;
    case ROPE_NODE_BRANCH:
        rope_branch_delete(node);
        break;
    }
}

RopeNode *rope_node_copy(RopeNode *node) {
    if (node) {
        (*node->ref_count)++;
    }
    return node;
}

Vec_char *rope_node_substr(RopeNode *node, size_t start, size_t length) {
    if (!node) {
        return vec_char_new();
    }

    switch (node->kind) {
    case ROPE_NODE_LEAF:
        return rope_leaf_substr(node, start, length);
    case ROPE_NODE_BRANCH:
        return rope_branch_substr(node, start, length);
    }

    assert(false);
}

size_t rope_node_length(RopeNode *node) {
    if (!node) {
        return 0;
    }

    return node->length;
}

size_t rope_node_lf_cnt(RopeNode *node) {
    if (!node) {
        return 0;
    }

    return node->lf_count;
}