#pragma once

#include "editor.h"

typedef enum {
    VIEW_EDIT,
    VIEW_BROWSE,
} ViewType;

typedef union {
    Editor editor;
} View;

typedef struct {
    ViewType view_type;
    View view;
} App;

void app_render(App app);