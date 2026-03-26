#ifndef POWERUP_H
#define POWERUP_H

#include "raylib.h"
#include "effects.h"
#include <stdbool.h>

#define MAX_WORLD_POWERUPS 8
#define POWERUP_SPAWN_EVERY 1500.0f
#define POWERUP_PICKUP_RADIUS 22.0f
#define POWERUP_DRAW_RADIUS  11
#define PU_JETPACK_TIME  5.0f
#define PU_BOOTS_TIME   15.0f
#define PU_ELIXIR_TIME  10.0f
#define PU_FEATHER_TIME 10.0f

typedef enum {
    PU_NONE    = -1,
    PU_JETPACK =  0,
    PU_HALO,
    PU_BOOTS,
    PU_ELIXIR,
    PU_FEATHER,
    PU_TYPE_COUNT
} PowerupType;

typedef struct {
    float       x, y;
    PowerupType type;
    bool        active;
} WorldPowerup;

typedef struct {
    WorldPowerup world[MAX_WORLD_POWERUPS];
    int          worldCount;
    float        nextSpawnY;
} PowerupSystem;

void PowerupSystem_Init(PowerupSystem *ps, float startWorldY);
void PowerupSystem_Reset(PowerupSystem *ps, float startWorldY);
void PowerupSystem_Update(PowerupSystem *ps, float cameraOffsetY, Vector2 playerPos, ActiveEffects *fx);
void PowerupSystem_Draw(PowerupSystem *ps, float cameraOffsetY);
void PowerupSystem_DrawHUD(PowerupSystem *ps, ActiveEffects *fx);

#endif
