#include <stdio.h>

#include "app.h"
#include "constants.h"
#include "raylib.h"

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "jaledit");

    App app;
    app.view_type = VIEW_EDIT;
    app.view.editor = editor;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        app_render(app);
        EndDrawing();
    }
}