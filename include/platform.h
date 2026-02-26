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

typedef struct {
  float x, y;
  bool active;
} Platform;

typedef struct {
  Platform items[MAX_PLATFORMS];
  int count;
  float highestY;
} PlatformList;

void PlatformList_Init(PlatformList *pl);
void PlatformList_Update(PlatformList *pl, float cameraOffsetY);
void PlatformList_Draw(PlatformList *pl, float cameraOffsetY);
bool PlatformList_CheckCollision(PlatformList *pl, Rectangle playerRect,
                                 float playerVelY, float cameraOffsetY);

#endif
