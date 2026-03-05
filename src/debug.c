#ifdef DEBUG_BUILD

#include "debug.h"
#include "game.h"
#include "raylib.h"
#include <stdio.h>

static void warpToMilestone(Game *g, int milestone) {
    int targetScore = milestone * MILESTONE_INTERVAL + 10;
    g->score         = targetScore;
    g->lastMilestone = milestone;
    g->cameraOffsetY = 0.0f;
    PlatformList_Init(&g->platforms);
    g->platforms.milestone = milestone;
    Player_Init(&g->player, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 120.0f);
}

void Debug_Update(Game *g) {
    if (g->state != STATE_PLAYING) return;

    if (IsKeyPressed(KEY_ONE))   warpToMilestone(g, 1);
    if (IsKeyPressed(KEY_TWO))   warpToMilestone(g, 2);
    if (IsKeyPressed(KEY_THREE)) warpToMilestone(g, 3);
    if (IsKeyPressed(KEY_FOUR))  warpToMilestone(g, 4);
    if (IsKeyPressed(KEY_FIVE))  warpToMilestone(g, 5);
    if (IsKeyPressed(KEY_SIX))   warpToMilestone(g, 6);
    if (IsKeyPressed(KEY_SEVEN)) warpToMilestone(g, 7);
}

void Debug_Draw(Game *g) {
    if (g->state != STATE_PLAYING) return;
    int panelW = 250, panelH = 150;
    int panelX = SCREEN_WIDTH - panelW - 8;
    int panelY = 8;

    DrawRectangle(panelX, panelY, panelW, panelH, Fade(BLACK, 0.65f));
    DrawRectangleLinesEx((Rectangle){panelX, panelY, panelW, panelH},
                         1, Fade((Color){255, 80, 80, 255}, 0.8f));
    DrawText("DEBUG", panelX + 8, panelY + 6, 14,
             (Color){255, 80, 80, 255});

    char buf[48];
    snprintf(buf, sizeof(buf), "Milestone: %d", g->lastMilestone);
    DrawText(buf, panelX + 8, panelY + 24, 13, WHITE);

    static const char *labels[7] = {
        "[1] Moving H",
        "[2] Flicker",
        "[3] Moving Flicker + Moving HV",
        "[4] Teleport + Moving V",
        "[5] Moving V Flicker + Moving Teleport",
        "[6] Disable Normal",
        "[7] Disable Fragile",
    };
    for (int i = 0; i < 7; i++) {
        Color c = (i < g->lastMilestone)
                      ? (Color){100, 200, 100, 255}
                      : (i == g->lastMilestone)
                            ? (Color){255, 220, 50, 255}
                            : GRAY;
        DrawText(labels[i], panelX + 8, panelY + 42 + i * 14, 12, c);
    }
}

#endif
