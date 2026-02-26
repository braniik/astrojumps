#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#define PLAYER_WIDTH 36
#define PLAYER_HEIGHT 36
#define GRAVITY 0.35f
#define BOUNCE_FORCE -13.0f
#define MOVE_SPEED 4.5f
#define MAX_FALL_SPEED 12.0f
#define ROTATION_SPEED 8.0f

typedef struct {
  Vector2 pos;
  Vector2 vel;
  float rotation;
  bool alive;
} Player;

void Player_Init(Player *p, float startX, float startY);
void Player_Update(Player *p, float moveDir); // moveDir: -1, 0, +1
void Player_Bounce(Player *p);
void Player_Draw(Player *p, float cameraOffsetY);
bool Player_IsBelowScreen(Player *p, float cameraOffsetY);

#endif
