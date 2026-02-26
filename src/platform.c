// generovani platforem, nekonecne scrollovani, detekcia ci pristanes a take

#include "platform.h"
#include <stdlib.h>

static void spawnPlatform(PlatformList *pl) {
  if (pl->count >= MAX_PLATFORMS)
    return;

  int sw = GetScreenWidth();
  float gap =
      PLATFORM_GAP_MIN + (rand() % (int)(PLATFORM_GAP_MAX - PLATFORM_GAP_MIN));
  float newY = pl->highestY - gap;
  float newX =
      PLATFORM_MARGIN + (rand() % (sw - PLATFORM_WIDTH - PLATFORM_MARGIN * 2));

  pl->items[pl->count] = (Platform){newX, newY, true};
  pl->count++;
  pl->highestY = newY;
}

void PlatformList_Init(PlatformList *pl) {
  pl->count = 0;
  pl->highestY = 0.0f;

  int sh = GetScreenHeight();

  pl->items[pl->count++] = (Platform){GetScreenWidth() / 2 - PLATFORM_WIDTH / 2,
                                      (float)(sh - 80), true};
  pl->highestY = (float)(sh - 80);

  while (pl->highestY > -(float)sh) {
    spawnPlatform(pl);
  }
}

void PlatformList_Update(PlatformList *pl, float cameraOffsetY) {
  int sh = GetScreenHeight();

  for (int i = 0; i < pl->count; i++) {
    float screenY = pl->items[i].y + cameraOffsetY;
    if (screenY > sh + PLATFORM_HEIGHT) {
      pl->items[i].active = false;
    }
  }

  int w = 0;
  for (int i = 0; i < pl->count; i++) {
    if (pl->items[i].active) {
      pl->items[w++] = pl->items[i];
    }
  }
  pl->count = w;

  float cameraTop = -cameraOffsetY;
  while (pl->highestY > cameraTop - (float)sh) {
    spawnPlatform(pl);
  }
}

void PlatformList_Draw(PlatformList *pl, float cameraOffsetY) {
  for (int i = 0; i < pl->count; i++) {
    if (!pl->items[i].active)
      continue;
    float screenY = pl->items[i].y + cameraOffsetY;
    DrawRectangle((int)pl->items[i].x, (int)screenY, PLATFORM_WIDTH,
                  PLATFORM_HEIGHT, (Color){60, 120, 220, 255});
    DrawRectangle((int)pl->items[i].x, (int)screenY, PLATFORM_WIDTH, 3,
                  (Color){120, 180, 255, 255});
  }
}

bool PlatformList_CheckCollision(PlatformList *pl, Rectangle playerRect,
                                 float playerVelY, float cameraOffsetY) {
  if (playerVelY <= 0)
    return false;

  float playerFeet = playerRect.y + playerRect.height;
  float playerFeetPrev = playerFeet - playerVelY;

  for (int i = 0; i < pl->count; i++) {
    if (!pl->items[i].active)
      continue;

    float px = pl->items[i].x;
    float py = pl->items[i].y + cameraOffsetY;
    float pw = PLATFORM_WIDTH;

    bool overlapX =
        (playerRect.x + playerRect.width > px) && (playerRect.x < px + pw);
    bool crossedTop = (playerFeetPrev <= py) && (playerFeet >= py);

    if (overlapX && crossedTop)
      return true;
  }
  return false;
}
