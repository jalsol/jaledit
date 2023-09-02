#pragma once

typedef struct Key {
    int modifier;
    int key;
} Key;

typedef enum EditorMode {
    EDITOR_NORMAL,
    EDITOR_INSERT,
    EDITOR_VISUAL,
    EDITOR_BUFFER_LIST,
    EDITOR_FINDER,
} EditorMode;

struct Editor;
typedef struct Editor Editor;

Editor *editor_new();
Editor *editor_new_from_file(const char *filename);
void editor_delete(Editor *editor);

void editor_update(Editor *editor);
void editor_render(Editor *editor);
void editor_open(Editor *editor, const char *filename);