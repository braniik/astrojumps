#include "platform.h"
#include "raylib.h"
#include <stdlib.h>

#define FLICKER_SOLID_TIME 1.2f
#define FLICKER_DARK_TIME 0.7f
#define MOVE_SPEED_MIN 1.2f
#define MOVE_SPEED_MAX 2.8f
#define MOVE_V_RANGE 55.0f
#define TELEPORT_INTERVAL 2.4f

static const Color COL_NORMAL_BODY = { 55, 38, 255, 255 };
static const Color COL_NORMAL_SHINE = { 120, 180, 255, 255 };
static const Color COL_FRAGILE_BODY = { 20, 162, 255, 255 };
static const Color COL_FLICKER_ON = { 46, 255, 56, 255 };
static const Color COL_FLICKER_SHINE = { 125, 255, 132, 255 };
static const Color COL_FLICKER_OFF = { 25, 102, 29, 160 };
static const Color COL_FLICKER_ON_H = { 233, 255, 33, 255 };
static const Color COL_FLICKER_SHN_H = { 243, 255, 125, 255 };
static const Color COL_FLICKER_OFF_H = { 101, 110, 24, 160 };
static const Color COL_FLICKER_ON_V = { 255, 101, 0, 255 };
static const Color COL_FLICKER_SHN_V = { 255, 183, 135, 255 };
static const Color COL_FLICKER_OFF_V = { 115, 58, 21, 160 };
static const Color COL_MOVING_BODY_H = { 20, 255, 224, 255 };
static const Color COL_MOVING_SHINE_H = { 128, 255, 240, 255 };
static const Color COL_MOVING_BODY_HV = { 176, 0, 0, 255 };
static const Color COL_MOVING_SHN_HV = { 255, 51, 51, 255 };
static const Color COL_TELEPORT_BODY = { 149, 0, 255, 255 };
static const Color COL_TELEPORT_SHINE = { 199, 120, 255, 255 };

static float rand_float(float lo, float hi) {
    return lo + (float)(rand() % 1000) / 1000.0f * (hi - lo);
}

static PlatformType pickType(int milestone) {
    PlatformType pool[10];
    int n = 0;

    if (milestone < 6) {
        pool[n++] = PLAT_NORMAL;
        pool[n++] = PLAT_NORMAL;
        pool[n++] = PLAT_FRAGILE;
        if (milestone >= 1) pool[n++] = PLAT_MOVING_H;
        if (milestone >= 2) pool[n++] = PLAT_FLICKER;
        if (milestone >= 3) pool[n++] = PLAT_MOVING_HV;
        if (milestone >= 3) pool[n++] = PLAT_FLICKER_H;
        if (milestone >= 4) pool[n++] = PLAT_TELEPORT;
        if (milestone >= 5) pool[n++] = PLAT_FLICKER_V;
    } else if (milestone < 7) {
        pool[n++] = PLAT_FRAGILE;
        pool[n++] = PLAT_FRAGILE;
        if (milestone >= 1) pool[n++] = PLAT_MOVING_H;
        if (milestone >= 2) pool[n++] = PLAT_FLICKER;
        if (milestone >= 3) pool[n++] = PLAT_MOVING_HV;
        if (milestone >= 3) pool[n++] = PLAT_FLICKER_H;
        if (milestone >= 5) pool[n++] = PLAT_FLICKER_V;
    } else {
        if (milestone >= 1) pool[n++] = PLAT_MOVING_H;
        if (milestone >= 2) pool[n++] = PLAT_FLICKER;
        if (milestone >= 3) pool[n++] = PLAT_MOVING_HV;
        if (milestone >= 3) pool[n++] = PLAT_FLICKER_H;
        if (milestone >= 5) pool[n++] = PLAT_FLICKER_V;
    }
    return pool[rand() % n];
}

static void initPlatform(Platform *p, float x, float y, PlatformType type) {
    *p = (Platform){0};
    p->x = x;
    p->y = y;
    p->active = true;
    p->type = type;
    p->flickerSolid = true;
    p->baseY = y;
    p->teleportTimer = TELEPORT_INTERVAL;

    switch (type) {
        case PLAT_FLICKER:
            p->flickerTimer = rand_float(0.0f, FLICKER_SOLID_TIME);
            break;

        case PLAT_FLICKER_H:
            p->flickerTimer = rand_float(0.0f, FLICKER_SOLID_TIME);
            p->velX = rand_float(MOVE_SPEED_MIN, MOVE_SPEED_MAX);
            if (rand() % 2) p->velX = -p->velX;
            break;

        case PLAT_FLICKER_V:
            p->flickerTimer = rand_float(0.0f, FLICKER_SOLID_TIME);
            p->velY = rand_float(0.4f, 1.0f);
            if (rand() % 2) p->velY = -p->velY;
            break;

        case PLAT_MOVING_H:
            p->velX = rand_float(MOVE_SPEED_MIN, MOVE_SPEED_MAX);
            if (rand() % 2) p->velX = -p->velX;
            break;

        case PLAT_MOVING_HV:
            p->velX = rand_float(MOVE_SPEED_MIN, MOVE_SPEED_MAX);
            if (rand() % 2) p->velX = -p->velX;
            p->velY = rand_float(0.4f, 1.0f);
            if (rand() % 2) p->velY = -p->velY;
            break;

        default: break;
    }
}

static void spawnPlatform(PlatformList *pl) {
    if (pl->count >= MAX_PLATFORMS) return;
    int sw = GetScreenWidth();
    float gap = rand_float(PLATFORM_GAP_MIN, PLATFORM_GAP_MAX);
    float newY = pl->highestY - gap;
    float newX = PLATFORM_MARGIN + (float)(rand() % (sw - PLATFORM_WIDTH - PLATFORM_MARGIN * 2));

    initPlatform(&pl->items[pl->count], newX, newY, pickType(pl->milestone));
    pl->count++;
    pl->highestY = newY;
}

void PlatformList_Init(PlatformList *pl) {
    pl->count = 0;
    pl->highestY = 0.0f;
    pl->milestone = 0;

    int sh = GetScreenHeight();
    initPlatform(&pl->items[pl->count++],
                 (float)(GetScreenWidth() / 2 - PLATFORM_WIDTH / 2),
                 (float)(sh - 80),
                 PLAT_NORMAL);
    pl->highestY = (float)(sh - 80);

    while (pl->highestY > -(float)sh)
        spawnPlatform(pl);
}

void PlatformList_Update(PlatformList *pl, float cameraOffsetY) {
    float dt = GetFrameTime();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    for (int i = 0; i < pl->count; i++) {
        Platform *p = &pl->items[i];
        if (!p->active) continue;

        switch (p->type) {

            case PLAT_FRAGILE:
                if (p->used) p->active = false;
                break;

            case PLAT_FLICKER:
                p->flickerTimer -= dt;
                if (p->flickerTimer <= 0.0f) {
                    p->flickerSolid = !p->flickerSolid;
                    p->flickerTimer = p->flickerSolid ? FLICKER_SOLID_TIME : FLICKER_DARK_TIME;
                }
                break;

            case PLAT_FLICKER_H:
                p->flickerTimer -= dt;
                if (p->flickerTimer <= 0.0f) {
                    p->flickerSolid = !p->flickerSolid;
                    p->flickerTimer = p->flickerSolid ? FLICKER_SOLID_TIME : FLICKER_DARK_TIME;
                }
                p->x += p->velX;
                if (p->x < PLATFORM_MARGIN) { p->x = PLATFORM_MARGIN; p->velX = -p->velX; }
                if (p->x > sw - PLATFORM_WIDTH - PLATFORM_MARGIN) {
                    p->x = (float)(sw - PLATFORM_WIDTH - PLATFORM_MARGIN); p->velX = -p->velX;
                }
                break;

            case PLAT_FLICKER_V:
                p->flickerTimer -= dt;
                if (p->flickerTimer <= 0.0f) {
                    p->flickerSolid = !p->flickerSolid;
                    p->flickerTimer = p->flickerSolid ? FLICKER_SOLID_TIME : FLICKER_DARK_TIME;
                }
                p->y += p->velY;
                if (p->y > p->baseY + MOVE_V_RANGE) { p->y = p->baseY + MOVE_V_RANGE; p->velY = -p->velY; }
                if (p->y < p->baseY - MOVE_V_RANGE) { p->y = p->baseY - MOVE_V_RANGE; p->velY = -p->velY; }
                break;

            case PLAT_MOVING_H:
                p->x += p->velX;
                if (p->x < PLATFORM_MARGIN) { p->x = PLATFORM_MARGIN; p->velX = -p->velX; }
                if (p->x > sw - PLATFORM_WIDTH - PLATFORM_MARGIN) {
                    p->x = (float)(sw - PLATFORM_WIDTH - PLATFORM_MARGIN); p->velX = -p->velX;
                }
                break;

            case PLAT_MOVING_HV:
                p->x += p->velX;
                p->y += p->velY;
                if (p->x < PLATFORM_MARGIN) { p->x = PLATFORM_MARGIN; p->velX = -p->velX; }
                if (p->x > sw - PLATFORM_WIDTH - PLATFORM_MARGIN) {
                    p->x = (float)(sw - PLATFORM_WIDTH - PLATFORM_MARGIN); p->velX = -p->velX;
                }
                if (p->y > p->baseY + MOVE_V_RANGE) { p->y = p->baseY + MOVE_V_RANGE; p->velY = -p->velY; }
                if (p->y < p->baseY - MOVE_V_RANGE) { p->y = p->baseY - MOVE_V_RANGE; p->velY = -p->velY; }
                break;

            case PLAT_TELEPORT:
                p->teleportTimer -= dt;
                if (p->teleportTimer <= 0.0f) {
                    p->teleportTimer = TELEPORT_INTERVAL;
                    float worldTop = -cameraOffsetY - (float)sh * 0.1f;
                    float worldBottom = -cameraOffsetY + (float)sh * 0.9f;
                    p->x = PLATFORM_MARGIN + (float)(rand() % (sw - PLATFORM_WIDTH - PLATFORM_MARGIN * 2));
                    p->y = worldTop + rand_float(0.0f, worldBottom - worldTop);
                }
                break;

            default: break;
        }

        if (p->y + cameraOffsetY > sh + PLATFORM_HEIGHT)
            p->active = false;
    }

    int w = 0;
    for (int i = 0; i < pl->count; i++)
        if (pl->items[i].active) pl->items[w++] = pl->items[i];
    pl->count = w;

    float cameraTop = -cameraOffsetY;
    while (pl->highestY > cameraTop - (float)sh)
        spawnPlatform(pl);
}

static void drawDotted(int x, int y, Color body) {
    int segW = 12;
    int gapW = 8;
    int total = PLATFORM_WIDTH;
    for (int cx = x; cx < x + total; cx += segW + gapW) {
        int w = segW;
        if (cx + w > x + total) w = x + total - cx;
        DrawRectangle(cx, y, w, PLATFORM_HEIGHT, body);
    }
    for (int cx = x; cx < x + total; cx += segW + gapW) {
        int w = segW;
        if (cx + w > x + total) w = x + total - cx;
        DrawRectangle(cx, y, w, 3, Fade(WHITE, 0.35f));
    }
}

void PlatformList_Draw(PlatformList *pl, float cameraOffsetY) {
    for (int i = 0; i < pl->count; i++) {
        Platform *p = &pl->items[i];
        if (!p->active) continue;

        int sx = (int)p->x;
        int sy = (int)(p->y + cameraOffsetY);

        switch (p->type) {

            case PLAT_NORMAL:
                DrawRectangle(sx, sy, PLATFORM_WIDTH, PLATFORM_HEIGHT, COL_NORMAL_BODY);
                DrawRectangle(sx, sy, PLATFORM_WIDTH, 3, COL_NORMAL_SHINE);
                break;

            case PLAT_FRAGILE:
                drawDotted(sx, sy, COL_FRAGILE_BODY);
                break;

            case PLAT_FLICKER:
                if (p->flickerSolid) {
                    DrawRectangle(sx, sy, PLATFORM_WIDTH, PLATFORM_HEIGHT, COL_FLICKER_ON);
                    DrawRectangle(sx, sy, PLATFORM_WIDTH, 3, COL_FLICKER_SHINE);
                } else {
                    DrawRectangle(sx, sy, PLATFORM_WIDTH, PLATFORM_HEIGHT, COL_FLICKER_OFF);
                }
                break;

            case PLAT_FLICKER_H:
                if (p->flickerSolid) {
                    DrawRectangle(sx, sy, PLATFORM_WIDTH, PLATFORM_HEIGHT, COL_FLICKER_ON_H);
                    DrawRectangle(sx, sy, PLATFORM_WIDTH, 3, COL_FLICKER_SHN_H);
                } else {
                    DrawRectangle(sx, sy, PLATFORM_WIDTH, PLATFORM_HEIGHT, COL_FLICKER_OFF_H);
                }
                break;

            case PLAT_FLICKER_V:
                if (p->flickerSolid) {
                    DrawRectangle(sx, sy, PLATFORM_WIDTH, PLATFORM_HEIGHT, COL_FLICKER_ON_V);
                    DrawRectangle(sx, sy, PLATFORM_WIDTH, 3, COL_FLICKER_SHN_V);
                } else {
                    DrawRectangle(sx, sy, PLATFORM_WIDTH, PLATFORM_HEIGHT, COL_FLICKER_OFF_V);
                }
                break;

            case PLAT_MOVING_H:
                DrawRectangle(sx, sy, PLATFORM_WIDTH, PLATFORM_HEIGHT, COL_MOVING_BODY_H);
                DrawRectangle(sx, sy, PLATFORM_WIDTH, 3, COL_MOVING_SHINE_H);
                break;

            case PLAT_MOVING_HV:
                DrawRectangle(sx, sy, PLATFORM_WIDTH, PLATFORM_HEIGHT, COL_MOVING_BODY_HV);
                DrawRectangle(sx, sy, PLATFORM_WIDTH, 3, COL_MOVING_SHN_HV);
                break;

            case PLAT_TELEPORT:
                DrawRectangle(sx, sy, PLATFORM_WIDTH, PLATFORM_HEIGHT, COL_TELEPORT_BODY);
                DrawRectangle(sx, sy, PLATFORM_WIDTH, 3, COL_TELEPORT_SHINE);
                break;
        }
    }
}

bool PlatformList_CheckCollision(PlatformList *pl, Rectangle playerRect,
                                 float playerVelY, float cameraOffsetY) {
    if (playerVelY <= 0.0f) return false;

    float feet = playerRect.y + playerRect.height;
    float feetPrev = feet - playerVelY;

    for (int i = 0; i < pl->count; i++) {
        Platform *p = &pl->items[i];
        if (!p->active) continue;
        if ((p->type == PLAT_FLICKER ||
             p->type == PLAT_FLICKER_H ||
             p->type == PLAT_FLICKER_V) && !p->flickerSolid) continue;

        float px = p->x;
        float py = p->y + cameraOffsetY;

        bool overlapX = (playerRect.x + playerRect.width > px) &&
                          (playerRect.x < px + PLATFORM_WIDTH);
        bool crossedTop = (feetPrev <= py) && (feet >= py);

        if (overlapX && crossedTop) {
            if (p->type == PLAT_FRAGILE) p->used = true;
            return true;
        }
    }
    return false;
}
