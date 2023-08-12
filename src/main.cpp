#include "constants.hpp"
#include "editor.hpp"
#include "raylib.h"

int main(int argc, char** argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    SetTargetFPS(144);

    Editor editor = argc > 1 ? Editor(argv[1]) : Editor("");

    InitWindow(constants::window::width, constants::window::height,
               "jaledit: just a little editor");
    SetExitKey(KEY_NULL);

    static Image icon = LoadImage("data/icon.png");
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground({239, 241, 245, 255});
        editor.render();

        EndDrawing();

        editor.update();
    }
    CloseWindow();
}
