#ifdef DEBUG_BUILD

#include "debug.h"
#include "game.h"
#include "raylib.h"
#include <stdio.h>

static void warpToMilestone(Game *g, int milestone) {
    int targetScore  = milestone * MILESTONE_INTERVAL + 10;
    g->score         = targetScore;
    g->lastMilestone = milestone;
    g->cameraOffsetY = 0.0f;
    PlatformList_Init(&g->platforms);
    g->platforms.milestone = milestone;
    Player_Init(&g->player, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 120.0f);
}

#define DBG_LAVA_DURATION      60.0f
#define DBG_WIND_DURATION      60.0f
#define DBG_FLASH_DURATION     60.0f
#define DBG_FLASH_SUBINTERVAL   5.0f
#define DBG_BLACKOUT_DURATION  60.0f
#define DBG_INVERSION_DURATION 60.0f

static void forceEvent(Game *g, EventType t) {
    EventSystem *es = &g->events;
    EventSlot   *s  = &es->slots[t];

    es->nextEventTimer = 60.0f;
    s->active = true;

    switch (t) {
        case EVT_LAVA:
            s->timer = DBG_LAVA_DURATION;
            s->data  = -g->cameraOffsetY + (float)SCREEN_HEIGHT + 50.0f;
            break;
        case EVT_WIND:
            s->timer = DBG_WIND_DURATION;
            s->data  = (s->data > 0.0f) ? -1.0f : 1.0f;
            break;
        case EVT_FLASH:
            s->timer = DBG_FLASH_DURATION;
            s->data  = DBG_FLASH_SUBINTERVAL;
            break;
        case EVT_BLACKOUT:
            s->timer = DBG_BLACKOUT_DURATION;
            s->data  = 0.0f;
            break;
        case EVT_INVERSION:
            s->timer = DBG_INVERSION_DURATION;
            break;
        default: break;
    }
}

void Debug_Update(Game *g) {
    if (g->state != STATE_PLAYING) return;
    if (IsKeyPressed(KEY_F1)) warpToMilestone(g, 1);
    if (IsKeyPressed(KEY_F2)) warpToMilestone(g, 2);
    if (IsKeyPressed(KEY_F3)) warpToMilestone(g, 3);
    if (IsKeyPressed(KEY_F4)) warpToMilestone(g, 4);
    if (IsKeyPressed(KEY_F5)) warpToMilestone(g, 5);
    if (IsKeyPressed(KEY_F6)) warpToMilestone(g, 6);
    if (IsKeyPressed(KEY_F7)) warpToMilestone(g, 7);
    if (IsKeyPressed(KEY_ONE))   forceEvent(g, EVT_LAVA);
    if (IsKeyPressed(KEY_TWO))   forceEvent(g, EVT_WIND);
    if (IsKeyPressed(KEY_THREE)) forceEvent(g, EVT_FLASH);
    if (IsKeyPressed(KEY_FOUR))  forceEvent(g, EVT_BLACKOUT);
    if (IsKeyPressed(KEY_FIVE))  forceEvent(g, EVT_INVERSION);
}

void Debug_Draw(Game *g) {
    if (g->state != STATE_PLAYING) return;

    int panelW = 220, panelH = 262;
    int panelX = SCREEN_WIDTH  - panelW - 8;
    int panelY = 8;

    DrawRectangle(panelX, panelY, panelW, panelH, Fade(BLACK, 0.65f));
    DrawRectangleLinesEx((Rectangle){panelX, panelY, panelW, panelH},
                         1, Fade((Color){255, 80, 80, 255}, 0.8f));
    DrawText("DEBUG", panelX + 8, panelY + 6, 14, (Color){255, 80, 80, 255});

    char buf[48];
    snprintf(buf, sizeof(buf), "Milestone: %d", g->lastMilestone);
    DrawText(buf, panelX + 8, panelY + 24, 13, WHITE);

    static const char *mLabels[7] = {
        "[F1] moving H",
        "[F2] flicker",
        "[F3] flicker H + moving HV",
        "[F4] teleport + flicker V",
        "[F5] all types",
        "[F6] no normals",
        "[F7] no fragile",
    };
    for (int i = 0; i < 7; i++) {
        Color c = (i < g->lastMilestone)
                      ? (Color){100, 200, 100, 255}
                      : (i == g->lastMilestone)
                            ? (Color){255, 220,  50, 255}
                            : GRAY;
        DrawText(mLabels[i], panelX + 8, panelY + 42 + i * 14, 12, c);
    }

    int divY = panelY + 42 + 7 * 14 + 4;
    DrawLine(panelX + 6, divY, panelX + panelW - 6, divY, Fade(WHITE, 0.2f));

    static const char *eLabels[EVT_COUNT] = {
        "[1] Lava",
        "[2] Wind",
        "[3] Flash",
        "[4] Blackout",
        "[5] Inversion",
    };
    static const Color eColors[EVT_COUNT] = {
        {255,  80,  20, 255},
        {100, 200, 255, 255},
        {255, 255, 180, 255},
        {160, 160, 160, 255},
        {200, 100, 255, 255},
    };

    int ey = divY + 5;
    for (int i = 0; i < EVT_COUNT; i++) {
        bool  active = g->events.slots[i].active;
        Color c      = active ? eColors[i] : GRAY;
        DrawText(eLabels[i], panelX + 8, ey + i * 14, 12, c);
        if (active) {
            float frac = g->events.slots[i].timer / 60.0f;
            if (frac > 1.0f) frac = 1.0f;
            int barW = 42;
            int barX = panelX + panelW - barW - 6;
            int barY = ey + i * 14 + 1;
            DrawRectangle(barX, barY, barW, 9, Fade(BLACK, 0.5f));
            DrawRectangle(barX, barY, (int)(barW * frac), 9, Fade(eColors[i], 0.8f));
        }
    }
}

#endif
