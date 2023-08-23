#pragma once

#include "common/vector.h"

#include <stddef.h>

enum RopeNodeKind {
    ROPE_NODE_LEAF,
    ROPE_NODE_BRANCH,
};

struct RopeNode;
typedef struct RopeNode RopeNode;

typedef struct RopeNodeSplit {
    RopeNode *left;
    RopeNode *right;
} RopeNodeSplit;

char rope_node_at(RopeNode *node, size_t index);
RopeNodeSplit rope_node_split(RopeNode *node, size_t index);
size_t rope_node_find_lf(RopeNode *node, size_t index);
RopeNode *rope_node_copy(RopeNode *node);
size_t rope_node_length(RopeNode *node);
Vec_char *rope_node_substr(RopeNode *node, size_t start, size_t length);
size_t rope_node_lf_cnt(RopeNode *node);

RopeNode *rope_leaf_new(const char *text, size_t length);
RopeNode *rope_branch_new(RopeNode *left, RopeNode *right);
void rope_node_delete(RopeNode *node);