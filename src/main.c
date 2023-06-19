#include <stdio.h>

#include "app.h"
#include "constants.h"
#include "raylib.h"

int main(int argc, char **argv) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "jaledit");

    editor = editor_construct(argv[1]);

    App app;
    app.view_type = VIEW_EDIT;
    app.view.editor = editor;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        app_render(&app);
        EndDrawing();
    }
    CloseWindow();

    editor_destruct(&editor);
}