#ifndef PLATFORM_H
#define PLATFORM_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_PLATFORMS 64
#define PLATFORM_WIDTH 80
#define PLATFORM_HEIGHT 14
#define PLATFORM_GAP_MIN 80
#define PLATFORM_GAP_MAX 140
#define PLATFORM_MARGIN 30

typedef enum {
    PLAT_NORMAL,
    PLAT_FRAGILE,
    PLAT_FLICKER,
    PLAT_FLICKER_H,
    PLAT_FLICKER_V,
    PLAT_MOVING_H,
    PLAT_MOVING_HV,
    PLAT_TELEPORT,
} PlatformType;

typedef struct {
  float x, y;
  bool active;
  PlatformType type;
  bool used;
  float flickerTimer;
  bool flickerSolid;
  float velX, velY;
  float baseY;
  float teleportTimer;
} Platform;

typedef struct {
  Platform items[MAX_PLATFORMS];
  int count;
  float highestY;
  int milestone;
} PlatformList;

void PlatformList_Init(PlatformList *pl);
void PlatformList_Update(PlatformList *pl, float cameraOffsetY);
void PlatformList_Draw(PlatformList *pl, float cameraOffsetY);
bool PlatformList_CheckCollision(PlatformList *pl, Rectangle playerRect,
                                 float playerVelY, float cameraOffsetY);

#endif
