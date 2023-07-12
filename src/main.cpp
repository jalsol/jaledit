#include <stdio.h>

#include "constants.hpp"
#include "raylib.h"

int main(int argc, char** argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    SetExitKey(KEY_NULL);
    SetTargetFPS(144);

    InitWindow(constants::window::width, constants::window::height, "jaledit");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
}