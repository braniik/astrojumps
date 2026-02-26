/*
 * Toto je cisto player fyzika nic zlozite co tu najdes je:
 * gravitacia
 * handlovani inputu
 * renderovani hraca (zatal len kostecka)
 */

#include "player.h"
#include <math.h>

void Player_Init(Player *p, float startX, float startY) {
  p->pos = (Vector2){startX, startY};
  p->vel = (Vector2){0.0f, 0.0f};
  p->rotation = 0.0f;
  p->alive = true;
}

void Player_Update(Player *p, float moveDir) {
  p->vel.x = moveDir * MOVE_SPEED;

  p->vel.y += GRAVITY;
  if (p->vel.y > MAX_FALL_SPEED)
    p->vel.y = MAX_FALL_SPEED;

  p->pos.x += p->vel.x;
  p->pos.y += p->vel.y;

  int sw = GetScreenWidth();
  if (p->pos.x < -PLAYER_WIDTH / 2)
    p->pos.x = sw + PLAYER_WIDTH / 2;
  if (p->pos.x > sw + PLAYER_WIDTH / 2)
    p->pos.x = -PLAYER_WIDTH / 2;

  p->rotation = 0.0f;
}

void Player_Bounce(Player *p) { p->vel.y = BOUNCE_FORCE; }

Rectangle Player_GetRect(Player *p) {
  return (Rectangle){p->pos.x - PLAYER_WIDTH / 2, p->pos.y - PLAYER_HEIGHT / 2,
                     PLAYER_WIDTH, PLAYER_HEIGHT};
}

void Player_Draw(Player *p, float cameraOffsetY) {
  float screenY = p->pos.y + cameraOffsetY;

  Vector2 centre = {p->pos.x, screenY};
  DrawRectanglePro((Rectangle){centre.x, centre.y, PLAYER_WIDTH, PLAYER_HEIGHT},
                   (Vector2){PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2}, p->rotation,
                   (Color){0, 200, 100, 255});  
}

bool Player_IsBelowScreen(Player *p, float cameraOffsetY) {
  float screenY = p->pos.y + cameraOffsetY;
  return screenY > GetScreenHeight() + PLAYER_HEIGHT;
}
