#include "editor.h"

#include <stdlib.h>

#include "buffer.h"
#include "raylib.h"
#include "utils.h"

Editor editor_construct(const char *file_path) {
    Editor editor;
    editor.mode = MODE_COMMAND;
    editor.buffer = buffer_construct(file_path);
    return editor;
}

void editor_render(Editor editor) {
    draw_text("lmao", (Vector2){0, 0}, BLACK, 20, 0);
}