#pragma once

#include <stdbool.h>

struct TrieNode;
typedef struct TrieNode TrieNode;

struct Editor;
typedef struct Editor Editor;

typedef enum TrieNodeKind {
    TRIE_NODE_LEAF,
    TRIE_NODE_BRANCH,
} TrieNodeKind;

TrieNodeKind trie_node_kind(TrieNode *node);
void trie_node_delete(TrieNode *node);
void trie_set_parent(TrieNode *node, TrieNode *parent);
TrieNode *trie_parent(TrieNode *node);

// start Branch

TrieNode *trie_branch_new();
TrieNode **trie_branch_child(TrieNode *node, char c);

// end Branch

// start Leaf

TrieNode *trie_leaf_new(bool editable, void (*func)(Editor *editor));
void trie_leaf_call(TrieNode *node, Editor *editor, bool editable);

// end Leaf