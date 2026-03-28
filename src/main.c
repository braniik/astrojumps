#include "game.h"
#include "raylib.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(TARGET_FPS);

    Image icon = LoadImage("assets/icon.png");
    if (icon.data) {
        SetWindowIcon(icon);
        UnloadImage(icon);
    }

    Game g;
    Game_Init(&g);

    while (!WindowShouldClose()) {
        Game_Update(&g);
        Game_Draw(&g);
    }

    Game_Cleanup(&g);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
