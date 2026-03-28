#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "effects.h"

#define PLAYER_WIDTH 54
#define PLAYER_HEIGHT 54
#define GRAVITY 0.35f
#define BOUNCE_FORCE -13.0f
#define MOVE_SPEED 4.5f
#define MAX_FALL_SPEED 12.0f

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float rotation;
    bool alive;
    Texture2D texture;
    Texture2D textureJetpack;
    Texture2D textureBoots;
} Player;

void Player_Load(Player *p);
void Player_Reset(Player *p, float startX, float startY);
void Player_Init(Player *p, float startX, float startY);

void Player_Update(Player *p, float moveDir, ActiveEffects *fx);
void Player_Bounce(Player *p, ActiveEffects *fx);
void Player_Draw(Player *p, float cameraOffsetY, ActiveEffects *fx);
bool Player_IsBelowScreen(Player *p, float cameraOffsetY);
void Player_StopSounds(void);
void Player_Unload(Player *p);

#endif
