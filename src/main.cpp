#include "constants.hpp"
#include "editor.hpp"
#include "raylib.h"

int main(int argc, char** argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    SetExitKey(KEY_NULL);
    SetTargetFPS(144);

    Editor editor = argc > 1 ? Editor(argv[1]) : Editor();

    InitWindow(constants::window::width, constants::window::height, "jaledit");

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        editor.render();

        EndDrawing();

        editor.update();
    }
    CloseWindow();
}