#include "editor.h"

#include <stdlib.h>

#include "buffer.h"
#include "raylib.h"

Editor editor_construct(const char *file_path) {
    Editor editor;
    editor.mode = MODE_COMMAND;
    editor.buffer = buffer_construct(file_path);
    return editor;
}

void editor_render(Editor editor) { DrawText("lmao", 0, 0, 20, BLACK); }