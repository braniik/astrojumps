// key bindovani proste co sa tyka teho tu najdes

#include "controls.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const KeyboardKey KEY_POOL[KEY_POOL_SIZE] = {
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I,
    KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
    KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z};

static const char KEY_CHARS[KEY_POOL_SIZE] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

void Controls_Init(Controls *c) {
  c->leftKey = KEY_A;
  c->rightKey = KEY_D;
  c->leftChar = 'A';
  c->rightChar = 'D';
}

void Controls_Shuffle(Controls *c) {
  int li, ri;
  do {
    li = rand() % KEY_POOL_SIZE;
    ri = rand() % KEY_POOL_SIZE;
  } while (li == ri);

  c->leftKey = KEY_POOL[li];
  c->rightKey = KEY_POOL[ri];
  c->leftChar = KEY_CHARS[li];
  c->rightChar = KEY_CHARS[ri];
}

float Controls_GetMoveDir(Controls *c) {
  float dir = 0.0f;
  if (IsKeyDown(c->leftKey))
    dir -= 1.0f;
  if (IsKeyDown(c->rightKey))
    dir += 1.0f;
  return dir;
}

void Controls_DrawHUD(Controls *c) {
  int screenW = GetScreenWidth();
  int screenH = GetScreenHeight();

  int boxW = 200, boxH = 38;
  int boxX = screenW / 2 - boxW / 2;
  int boxY = screenH - boxH - 10;

  DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.55f));
  DrawRectangleLinesEx((Rectangle){boxX, boxY, boxW, boxH}, 1,
                       Fade(WHITE, 0.4f));

  char buf[64];
  snprintf(buf, sizeof(buf), "[%c] LEFT    RIGHT [%c]", c->leftChar,
           c->rightChar);
  int tw = MeasureText(buf, 16);
  DrawText(buf, screenW / 2 - tw / 2, boxY + 11, 16, WHITE);
}
