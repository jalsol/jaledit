#pragma once

#include "buffer.h"
#include "keybind.h"

typedef enum {
    MODE_NORMAL,
    MODE_COMMAND,
    MODE_INSERT,
    MODE_VISUAL,
    MODE_REPLACE,
} Mode;

typedef struct {
    Buffer buffer;
    Mode mode;
    KeybindTrie keybind_trie;
} Editor;

Editor editor_construct(const char *file_path);
void editor_destruct(Editor *editor);
void editor_render(Editor *editor);
void editor_update(Editor *editor);
void editor_update_normal_mode(Editor *editor);
// void editor_update_insert_mode(Editor *editor);
void editor_keybind_trie_step(Editor *editor, char chr);
