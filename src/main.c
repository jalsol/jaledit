#include <stdio.h>

#include "app.h"
#include "constants.h"
#include "raylib.h"

extern Editor editor;

int main(int argc, char **argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    SetExitKey(KEY_NULL);
    SetTargetFPS(144);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "jaledit");

    if (argc >= 2) {
        editor = editor_construct(argv[1]);
    } else {
        editor = editor_construct(NULL);
    }

    keybind_trie_populate(&editor.keybind_trie);

    App app;
    app.view_type = VIEW_EDIT;
    app.view.editor = editor;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        app_render(&app);
        EndDrawing();

        app_update(&app);
    }
    CloseWindow();

    editor_destruct(&editor);
}