#include "node.h"

#include <stdbool.h>
#include <stdlib.h>

#define KEYBIND_MAX_CHILDREN (1 << 8)

struct TrieNode {
    TrieNodeKind kind;
    TrieNode *parent;

    union {
        struct {
            bool editable;
            void (*func)(void);
        } leaf;
        struct {
            TrieNode *children[KEYBIND_MAX_CHILDREN];
        } branch;
    };
};

// start Branch

TrieNode *trie_branch_new() {
    TrieNode *node = malloc(sizeof(TrieNode));
    node->kind = TRIE_NODE_BRANCH;
    node->parent = NULL;
    for (int i = 0; i < KEYBIND_MAX_CHILDREN; i++) {
        node->branch.children[i] = NULL;
    }
    return node;
}

void trie_branch_delete(TrieNode *node) {
    for (int i = 0; i < KEYBIND_MAX_CHILDREN; i++) {
        trie_node_delete(node->branch.children[i]);
    }

    free(node);
}

TrieNode **trie_branch_child(TrieNode *node, char c) { return &node->branch.children[c]; }

// end Branch

// start Leaf

TrieNode *trie_leaf_new(bool editable, void (*func)(void)) {
    TrieNode *node = malloc(sizeof(TrieNode));
    node->kind = TRIE_NODE_LEAF;
    node->parent = NULL;
    node->leaf.editable = editable;
    node->leaf.func = func;
    return node;
}

void trie_leaf_delete(TrieNode *node) { free(node); }

void trie_leaf_call(TrieNode *node, bool editable) {
    if (node->leaf.editable && !editable) {
        return;
    }

    node->leaf.func();
}

// end Leaf

TrieNodeKind trie_node_kind(TrieNode *node) { return node->kind; }

void trie_node_delete(TrieNode *node) {
    if (node == NULL) {
        return;
    }

    switch (node->kind) {
    case TRIE_NODE_BRANCH:
        trie_branch_delete(node);
        break;
    case TRIE_NODE_LEAF:
        trie_leaf_delete(node);
        break;
    }
}

void trie_set_parent(TrieNode *node, TrieNode *parent) { node->parent = parent; }

TrieNode *trie_parent(TrieNode *node) { return node->parent; }