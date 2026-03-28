#include "powerup.h"
#include <stdlib.h>
#include <stdio.h>

static Texture2D powerupTextures[PU_TYPE_COUNT];
static Sound sfxPickup;

static const Color PU_COLORS[PU_TYPE_COUNT] = {
    {255, 140, 30, 255}, // Jetpack
    {255, 230, 50, 255}, // Revive
    { 50, 220, 220, 255}, // Boots
    { 80, 220, 80, 255}, // Elixir
    {220, 220, 220, 255}, // Feather
};

static float rand_float(float lo, float hi) {
    return lo + (float)(rand() % 1000) / 1000.0f * (hi - lo);
}

static void spawnOne(PowerupSystem *ps, float worldY) {
    if (ps->worldCount >= MAX_WORLD_POWERUPS) return;
    int sw = GetScreenWidth();
    WorldPowerup *wp = &ps->world[ps->worldCount++];
    wp->x = rand_float(40.0f, (float)(sw - 40));
    wp->y = worldY;
    wp->type = (PowerupType)(rand() % PU_TYPE_COUNT);
    wp->active = true;
}

void PowerupSystem_Init(PowerupSystem *ps, float startWorldY) {
    ps->worldCount = 0;
    ps->nextSpawnY = startWorldY - POWERUP_SPAWN_EVERY;

    powerupTextures[PU_JETPACK] = LoadTexture("assets/sprites/jetpack.png");
    powerupTextures[PU_HALO] = LoadTexture("assets/sprites/revive.png");
    powerupTextures[PU_BOOTS] = LoadTexture("assets/sprites/boots.png");
    powerupTextures[PU_ELIXIR] = LoadTexture("assets/sprites/elixir.png");
    powerupTextures[PU_FEATHER] = LoadTexture("assets/sprites/feather.png");

    for (int i = 0; i < PU_TYPE_COUNT; i++)
        SetTextureFilter(powerupTextures[i], TEXTURE_FILTER_POINT);

    sfxPickup = LoadSound("assets/sfx/pickup.mp3");
}

void PowerupSystem_Reset(PowerupSystem *ps, float startWorldY) {
    ps->worldCount = 0;
    ps->nextSpawnY = startWorldY - POWERUP_SPAWN_EVERY;
}

void PowerupSystem_Update(PowerupSystem *ps, float cameraOffsetY,
                          Vector2 playerPos, ActiveEffects *fx) {
    float dt = GetFrameTime();
    int sh = GetScreenHeight();

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
            switch (wp->type) {
                case PU_JETPACK: fx->jetpackTimer = PU_JETPACK_TIME; break;
                case PU_HALO: fx->haloReady = true; break;
                case PU_BOOTS: fx->bootsTimer = PU_BOOTS_TIME; fx->bootsUsed = false; break;
                case PU_ELIXIR: fx->elixirTimer = PU_ELIXIR_TIME; break;
                case PU_FEATHER: fx->featherTimer = PU_FEATHER_TIME; break;
                default: break;
            }
            wp->active = false;
            PlaySound(sfxPickup);
        }
    }

    int w = 0;
    for (int i = 0; i < ps->worldCount; i++)
        if (ps->world[i].active) ps->world[w++] = ps->world[i];
    ps->worldCount = w;

    if (fx->jetpackTimer > 0) fx->jetpackTimer -= dt;
    if (fx->featherTimer > 0) fx->featherTimer -= dt;
    if (fx->elixirTimer > 0) fx->elixirTimer -= dt;
    if (fx->bootsTimer > 0) fx->bootsTimer -= dt;
    if (fx->haloReviveTimer > 0) fx->haloReviveTimer -= dt;
}

void PowerupSystem_Draw(PowerupSystem *ps, float cameraOffsetY) {
    for (int i = 0; i < ps->worldCount; i++) {
        WorldPowerup *wp = &ps->world[i];
        if (!wp->active) continue;

        int sx = (int)wp->x;
        int sy = (int)(wp->y + cameraOffsetY);
        Texture2D tex = powerupTextures[wp->type];
        float size = 32.0f;

        DrawCircle(sx, sy, size / 2.0f + 3.0f, Fade(WHITE, 0.2f));

        Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
        Rectangle dest = { (float)sx, (float)sy, size, size };
        Vector2 origin = { size / 2.0f, size / 2.0f };
        DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
    }
}

void PowerupSystem_DrawHUD(PowerupSystem *ps, ActiveEffects *fx) {
    (void)ps;
    char buf[32];
    int x0 = 10, ty = 60;

    if (fx->jetpackTimer > 0) { snprintf(buf, 32, "Jetpack %.1fs", fx->jetpackTimer); DrawText(buf, x0, ty, 11, PU_COLORS[PU_JETPACK]); ty += 13; }
    if (fx->featherTimer > 0) { snprintf(buf, 32, "Feather %.1fs", fx->featherTimer); DrawText(buf, x0, ty, 11, PU_COLORS[PU_FEATHER]); ty += 13; }
    if (fx->elixirTimer > 0) { snprintf(buf, 32, "Elixir %.1fs", fx->elixirTimer); DrawText(buf, x0, ty, 11, PU_COLORS[PU_ELIXIR]); ty += 13; }
    if (fx->bootsTimer > 0) { snprintf(buf, 32, "Boots %.1fs", fx->bootsTimer); DrawText(buf, x0, ty, 11, PU_COLORS[PU_BOOTS]); ty += 13; }
    if (fx->haloReady) { DrawText("Revive ready", x0, ty, 11, PU_COLORS[PU_HALO]); }
}

void PowerupSystem_Unload(void) {
    for (int i = 0; i < PU_TYPE_COUNT; i++) {
        UnloadTexture(powerupTextures[i]);
        powerupTextures[i] = (Texture2D){0};
    }
    UnloadSound(sfxPickup);
}
