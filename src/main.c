#include "raylib.h"

#include "common/constants.h"
#include "editor.h"

int main(int argc, char **argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    SetExitKey(KEY_NULL);
    SetTargetFPS(144);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "jaledit: just a little editor");

    Editor *editor =
        (argc > 1) ? editor_new_from_file(argv[1]) : editor_new_from_file("");

    Image icon = LoadImage("data/icon.png");
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground((Color){239, 241, 245, 255});
        editor_render(editor);

        EndDrawing();

        editor_update(editor);
    }
    CloseWindow();

    editor_delete(editor);
}