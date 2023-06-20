#include "keybind.h"

#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "constants.h"

KeybindTrie keybind_trie_construct() {
    KeybindTrie trie;
    trie.root = malloc(sizeof(KeybindTrieNode));
    memset(trie.root, 0, sizeof(KeybindTrieNode));
    trie.ptr = trie.root;
    return trie;
}

void keybind_trie_destruct(KeybindTrie *trie) {
    keybind_trie_node_destruct(trie->root);
    free(trie->root);
    trie->root = trie->ptr = NULL;
}

void keybind_trie_node_destruct(KeybindTrieNode *node) {
    for (int i = 0; i < CHAR_LIMIT; i++) {
        if (node->children[i] != NULL) {
            keybind_trie_node_destruct(node->children[i]);
            free(node->children[i]);
            node->children[i] = NULL;
        }
    }
}

void keybind_trie_populate(KeybindTrie *trie) {
    keybind_trie_insert(
        trie, "h", KEYBIND_BUFFER_MOVE_CURSOR,
        (KeybindHandlerArgs){.buffer_move_cursor = {.dx = -1, .dy = 0}});
    keybind_trie_insert(
        trie, "j", KEYBIND_BUFFER_MOVE_CURSOR,
        (KeybindHandlerArgs){.buffer_move_cursor = {.dx = 0, .dy = 1}});
    keybind_trie_insert(
        trie, "k", KEYBIND_BUFFER_MOVE_CURSOR,
        (KeybindHandlerArgs){.buffer_move_cursor = {.dx = 0, .dy = -1}});
    keybind_trie_insert(
        trie, "l", KEYBIND_BUFFER_MOVE_CURSOR,
        (KeybindHandlerArgs){.buffer_move_cursor = {.dx = 1, .dy = 0}});
    keybind_trie_insert(
        trie, "gg", KEYBIND_BUFFER_MOVE_CURSOR,
        (KeybindHandlerArgs){
            .buffer_move_cursor = {.dx = 0, .dy = -MAX_BUFFER_LINES}});
    keybind_trie_insert(
        trie, "G", KEYBIND_BUFFER_MOVE_CURSOR,
        (KeybindHandlerArgs){
            .buffer_move_cursor = {.dx = 0, .dy = MAX_BUFFER_LINES}});
    keybind_trie_insert(
        trie, "0", KEYBIND_BUFFER_MOVE_CURSOR,
        (KeybindHandlerArgs){
            .buffer_move_cursor = {.dx = -MAX_BUFFER_LINES, .dy = 0}});
    keybind_trie_insert(
        trie, "$", KEYBIND_BUFFER_MOVE_CURSOR,
        (KeybindHandlerArgs){
            .buffer_move_cursor = {.dx = MAX_BUFFER_LINES, .dy = 0}});
    keybind_trie_insert(trie, "w", KEYBIND_BUFFER_MOVE_TO_NEXT_WORD,
                        (KeybindHandlerArgs){.void_param = {}});
    keybind_trie_insert(trie, "b", KEYBIND_BUFFER_MOVE_TO_PREV_WORD,
                        (KeybindHandlerArgs){.void_param = {}});
}

void keybind_trie_insert(KeybindTrie *trie, const char *sequence,
                         KeybindHandlerType handler_type,
                         KeybindHandlerArgs handler_args) {
    KeybindTrieNode *node = trie->root;
    size_t seq_len = strlen(sequence);

    for (size_t i = 0; i < seq_len; i++) {
        char c = sequence[i];
        if (node->children[c] == NULL) {
            node->children[c] = malloc(sizeof(KeybindTrieNode));
            memset(node->children[c], 0, sizeof(KeybindTrieNode));
        }
        node = node->children[c];
    }

    node->handler_type = handler_type;
    node->handler_args = handler_args;
}
