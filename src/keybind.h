#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include "buffer.h"

typedef enum {
    KEYBIND_NONE,
    KEYBIND_BUFFER_MOVE_CURSOR,
} KeybindHandlerType;

typedef union {
    struct {
        int x;
        int y;
    } buffer_move_cursor;
} KeybindHandlerArgs;

typedef struct KeybindTrieNode {
    struct KeybindTrieNode *children[1 << 8];
    KeybindHandlerType handler_type;
    KeybindHandlerArgs handler_args;
} KeybindTrieNode;

typedef struct {
    KeybindTrieNode *root;
    KeybindTrieNode *ptr;
} KeybindTrie;

KeybindTrie keybind_trie_construct();
void keybind_trie_node_destruct(KeybindTrieNode *node);
void keybind_trie_destruct(KeybindTrie *trie);
void keybind_trie_populate(KeybindTrie *trie);

void keybind_trie_insert(KeybindTrie *trie, const char *sequence,
                         KeybindHandlerType handler_type,
                         KeybindHandlerArgs handler_args);
