#include "powerup.h"
#include <stdlib.h>
#include <stdio.h>

#define JETPACK_TIME  5.0f
#define BOOTS_TIME   15.0f
#define ELIXIR_TIME  10.0f
#define FEATHER_TIME 10.0f

static const Color PU_COLORS[PU_TYPE_COUNT] = {
    {255, 140,  30, 255},
    {255, 230,  50, 255},
    { 50, 220, 220, 255},
    { 80, 220,  80, 255},
    {220, 220, 220, 255},
};
static const char *PU_ICONS[PU_TYPE_COUNT]  = { "J", "O", "B", "E", "F" };
static const char *PU_NAMES[PU_TYPE_COUNT]  = { "Jetpack", "Halo", "Boots", "Elixir", "Feather" };

static float randF(float lo, float hi) {
    return lo + (float)(rand() % 1000) / 1000.0f * (hi - lo);
}

static void spawnOne(PowerupSystem *ps, float worldY) {
    if (ps->worldCount >= MAX_WORLD_POWERUPS) return;
    int sw = GetScreenWidth();
    WorldPowerup *wp = &ps->world[ps->worldCount++];
    wp->x     = randF(40.0f, (float)(sw - 40));
    wp->y     = worldY;
    wp->type  = (PowerupType)(rand() % PU_TYPE_COUNT);
    wp->active = true;
}

static void activateSlot(PowerupSystem *ps, int slot, ActiveEffects *fx) {
    if (slot < 0 || slot >= INVENTORY_SLOTS) return;
    PowerupType t = ps->slots[slot];
    if (t == PU_NONE) return;

    switch (t) {
        case PU_JETPACK: fx->jetpackTimer  = JETPACK_TIME;                    break;
        case PU_HALO:    fx->haloReady     = true;                            break;
        case PU_BOOTS:   fx->bootsTimer    = BOOTS_TIME; fx->bootsUsed = false; break;
        case PU_ELIXIR:  fx->elixirTimer   = ELIXIR_TIME;                     break;
        case PU_FEATHER: fx->featherTimer  = FEATHER_TIME;                    break;
        default: break;
    }
    ps->slots[slot] = PU_NONE;
}

void PowerupSystem_Init(PowerupSystem *ps, float startWorldY) {
    ps->worldCount = 0;
    for (int i = 0; i < INVENTORY_SLOTS; i++) ps->slots[i] = PU_NONE;
    ps->nextSpawnY = startWorldY - POWERUP_SPAWN_EVERY;
}

void PowerupSystem_Reset(PowerupSystem *ps, float startWorldY) {
    PowerupSystem_Init(ps, startWorldY);
}

void PowerupSystem_Update(PowerupSystem *ps, float cameraOffsetY,
                          Vector2 playerPos, ActiveEffects *fx) {
    float dt = GetFrameTime();
    int   sh = GetScreenHeight();
    float cameraTop = -cameraOffsetY;
    while (ps->nextSpawnY > cameraTop - (float)sh) {
        spawnOne(ps, ps->nextSpawnY);
        ps->nextSpawnY -= POWERUP_SPAWN_EVERY;
    }

    for (int i = 0; i < ps->worldCount; i++) {
        WorldPowerup *wp = &ps->world[i];
        if (!wp->active) continue;

        if (wp->y + cameraOffsetY > (float)sh + 40.0f) {
            wp->active = false;
            continue;
        }

        float dx = playerPos.x - wp->x;
        float dy = playerPos.y - wp->y;
        if (dx*dx + dy*dy < POWERUP_PICKUP_RADIUS * POWERUP_PICKUP_RADIUS) {
            for (int s = 0; s < INVENTORY_SLOTS; s++) {
                if (ps->slots[s] == PU_NONE) {
                    ps->slots[s] = wp->type;
                    wp->active   = false;
                    break;
                }
            }
        }
    }

    int w = 0;
    for (int i = 0; i < ps->worldCount; i++)
        if (ps->world[i].active) ps->world[w++] = ps->world[i];
    ps->worldCount = w;

    if (IsKeyPressed(KEY_ONE))   activateSlot(ps, 0, fx);
    if (IsKeyPressed(KEY_TWO))   activateSlot(ps, 1, fx);
    if (IsKeyPressed(KEY_THREE)) activateSlot(ps, 2, fx);
    if (fx->jetpackTimer    > 0) fx->jetpackTimer   -= dt;
    if (fx->featherTimer    > 0) fx->featherTimer   -= dt;
    if (fx->elixirTimer     > 0) fx->elixirTimer    -= dt;
    if (fx->bootsTimer      > 0) fx->bootsTimer     -= dt;
    if (fx->haloReviveTimer > 0) fx->haloReviveTimer -= dt;
}

void PowerupSystem_Draw(PowerupSystem *ps, float cameraOffsetY) {
    for (int i = 0; i < ps->worldCount; i++) {
        WorldPowerup *wp = &ps->world[i];
        if (!wp->active) continue;
        int   sx = (int)wp->x;
        int   sy = (int)(wp->y + cameraOffsetY);
        Color c  = PU_COLORS[wp->type];
        DrawCircle(sx, sy, POWERUP_DRAW_RADIUS + 4, Fade(c, 0.25f));
        DrawCircleLines(sx, sy, POWERUP_DRAW_RADIUS + 4, c);
        int tw = MeasureText(PU_ICONS[wp->type], 13);
        DrawText(PU_ICONS[wp->type], sx - tw/2, sy - 7, 13, c);
    }
}

void PowerupSystem_DrawHUD(PowerupSystem *ps, ActiveEffects *fx) {
    int x0 = 10, y0 = 60;
    int sw = 52, sh = 52, gap = 5;

    for (int i = 0; i < INVENTORY_SLOTS; i++) {
        int x = x0 + i * (sw + gap);
        DrawRectangle(x, y0, sw, sh, Fade(BLACK, 0.5f));
        DrawRectangleLinesEx((Rectangle){x, y0, sw, sh}, 1, Fade(WHITE, 0.28f));

        char num[4]; snprintf(num, sizeof(num), "[%d]", i + 1);
        DrawText(num, x + 3, y0 + 3, 10, GRAY);

        PowerupType t = ps->slots[i];
        if (t == PU_NONE) continue;

        Color c = PU_COLORS[t];
        int cx = x + sw/2, cy = y0 + 24;
        DrawCircle(cx, cy, 13, Fade(c, 0.28f));
        DrawCircleLines(cx, cy, 13, c);
        int tw = MeasureText(PU_ICONS[t], 15);
        DrawText(PU_ICONS[t], cx - tw/2, cy - 8, 15, c);
        int nw = MeasureText(PU_NAMES[t], 8);
        DrawText(PU_NAMES[t], cx - nw/2, y0 + 41, 8, Fade(c, 0.8f));
    }

    char buf[32];
    int ty = y0 + sh + 6;
    if (fx->jetpackTimer    > 0) { snprintf(buf,32,"Jetpack %.1fs",fx->jetpackTimer);  DrawText(buf,x0,ty,11,PU_COLORS[PU_JETPACK]);  ty+=13; }
    if (fx->featherTimer    > 0) { snprintf(buf,32,"Feather %.1fs",fx->featherTimer);  DrawText(buf,x0,ty,11,PU_COLORS[PU_FEATHER]);  ty+=13; }
    if (fx->elixirTimer     > 0) { snprintf(buf,32,"Elixir  %.1fs",fx->elixirTimer);   DrawText(buf,x0,ty,11,PU_COLORS[PU_ELIXIR]);   ty+=13; }
    if (fx->bootsTimer      > 0) { snprintf(buf,32,"Boots   %.1fs",fx->bootsTimer);    DrawText(buf,x0,ty,11,PU_COLORS[PU_BOOTS]);    ty+=13; }
    if (fx->haloReady)           { DrawText("Halo ready", x0, ty, 11, PU_COLORS[PU_HALO]); }
}
