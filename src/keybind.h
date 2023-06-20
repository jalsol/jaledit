#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include "buffer.h"
#include "constants.h"

typedef enum {
    KEYBIND_NONE,
    KEYBIND_BUFFER_MOVE_CURSOR,
    KEYBIND_BUFFER_MOVE_TO_NEXT_WORD,
    KEYBIND_BUFFER_MOVE_TO_PREV_WORD,
} KeybindHandlerType;

typedef union {
    struct {
        int dx;
        int dy;
    } buffer_move_cursor;
    struct {
    } buffer_move_to_next_word;
    struct {
    } buffer_move_to_prev_word;
} KeybindHandlerArgs;

typedef struct KeybindTrieNode {
    struct KeybindTrieNode *children[CHAR_LIMIT];
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
