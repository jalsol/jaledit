#pragma once

#include "buffer.h"

typedef enum {
    MODE_COMMAND,
    MODE_INSERT,
    MODE_VISUAL,
    MODE_REPLACE,
} Mode;

typedef struct {
    Buffer buffer;
    Mode mode;
} Editor;

static Editor editor;

Editor editor_construct(const char* file_path);
void editor_render(Editor editor);