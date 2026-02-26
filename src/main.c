// Asi najjednoduchsi file, proste nas startovnik, nejeb tu nic insi

#include "game.h"
#include "raylib.h"

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
  SetTargetFPS(TARGET_FPS);

  Game g;
  Game_Init(&g);

  while (!WindowShouldClose()) {
    Game_Update(&g);
    Game_Draw(&g);
  }

  Game_Cleanup(&g);
  CloseWindow();
  return 0;
}
