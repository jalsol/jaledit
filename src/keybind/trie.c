#include "trie.h"

#include "node.h"

#include <stdlib.h>

struct Keybind {
    TrieNode *root;
    TrieNode *current;
};

Keybind *keybind_new() {
    Keybind *keybind = malloc(sizeof(Keybind));
    keybind->root = trie_branch_new();
    keybind->current = keybind->root;
    return keybind;
}

void keybind_delete(Keybind *keybind) {
    trie_node_delete(keybind->root);
    free(keybind);
}

void keybind_add(Keybind *keybind, const char *key, size_t len, bool editable,
                 void (*func)(Editor *editor)) {
    TrieNode *current = keybind->root;

    for (size_t i = 0; i < len - 1; ++i) {
        TrieNode **child_ref = trie_branch_child(current, key[i]);

        if (*child_ref == NULL) {
            *child_ref = trie_branch_new();
        }

        trie_set_parent(*child_ref, current);
        current = *child_ref;
    }

    *trie_branch_child(current, key[len - 1]) = trie_leaf_new(editable, func);
}

void keybind_step(Keybind *keybind, Editor *editor, char c, bool editable) {
    if (keybind->current == NULL) {
        keybind_reset(keybind);
        return;
    }

    TrieNode **child_ref = trie_branch_child(keybind->current, c);
    if (*child_ref == NULL) {
        keybind_reset(keybind);
        return;
    }

    keybind->current = *child_ref;

    if (trie_node_kind(keybind->current) == TRIE_NODE_LEAF) {
        trie_leaf_call(keybind->current, editor, editable);
        keybind_reset(keybind);
    }
}

void keybind_reset(Keybind *keybind) { keybind->current = keybind->root; }