// menu, jestli sa hraje, game over, zakladne veci co sa tyka hry jak nazev rice

#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef DEBUG_BUILD
#include "debug.h"
#endif

static char  s_flashMsg[64] = {0};
static float s_flashTimer   = 0.0f;
#define FLASH_DURATION 2.8f

static void triggerFlash(const char *msg) {
    snprintf(s_flashMsg, sizeof(s_flashMsg), "%s", msg);
    s_flashTimer = FLASH_DURATION;
}

void Game_Init(Game *g) {
    srand((unsigned int)time(NULL));

    g->state         = STATE_MENU;
    g->cameraOffsetY = 0.0f;
    g->score         = 0;
    g->lastMilestone = 0;
    g->highScore     = 0;

    Controls_Init(&g->controls);
    Player_Init(&g->player, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 120.0f);
    PlatformList_Init(&g->platforms);
    PowerupSystem_Init(&g->powerups, (float)(SCREEN_HEIGHT - 120));
    EventSystem_Init(&g->events);
    g->fx = (ActiveEffects){0};
}

static void startGame(Game *g) {
    g->state         = STATE_PLAYING;
    g->cameraOffsetY = 0.0f;
    g->score         = 0;
    g->lastMilestone = 0;
    s_flashTimer     = 0.0f;

    Controls_Init(&g->controls);
    Player_Init(&g->player, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 120.0f);
    PlatformList_Init(&g->platforms);
    PowerupSystem_Reset(&g->powerups, (float)(SCREEN_HEIGHT - 120));
    EventSystem_Reset(&g->events);
    g->fx = (ActiveEffects){0};
}

void Game_Update(Game *g) {
    switch (g->state) {

    case STATE_MENU:
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            startGame(g);
        break;

    case STATE_PLAYING: {
        float windForce   = 0.0f;
        float lavaScreenY = 0.0f;
        EventSystem_Update(&g->events, g->cameraOffsetY, &windForce, &lavaScreenY);

        float moveDir = Controls_GetMoveDir(&g->controls);
        moveDir += windForce / MOVE_SPEED;
        Player_Update(&g->player, moveDir, &g->fx);

#ifdef DEBUG_BUILD
        Debug_Update(g);
#endif

        float playerScreenY = g->player.pos.y + g->cameraOffsetY;
        if (playerScreenY < SCREEN_HEIGHT / 2) {
            float diff        = SCREEN_HEIGHT / 2.0f - playerScreenY;
            g->cameraOffsetY += diff;
            g->score          = (int)(g->cameraOffsetY / 10);
        }

        float extra = (g->fx.elixirTimer > 0.0f) ? (PLAYER_WIDTH * 0.45f / 2.0f) : 0.0f;
        Rectangle pr = {
            g->player.pos.x - PLAYER_WIDTH / 2.0f - extra,
            g->player.pos.y - PLAYER_HEIGHT / 2.0f + g->cameraOffsetY,
            PLAYER_WIDTH  + extra * 2.0f,
            PLAYER_HEIGHT
        };
        if (PlatformList_CheckCollision(&g->platforms, pr,
                                        g->player.vel.y, g->cameraOffsetY)) {
            Player_Bounce(&g->player, &g->fx);
        }

        PlatformList_Update(&g->platforms, g->cameraOffsetY);
        PowerupSystem_Update(&g->powerups, g->cameraOffsetY,
                             g->player.pos, &g->fx);

        int milestoneReached = g->score / MILESTONE_INTERVAL;
        if (milestoneReached > g->lastMilestone) {
            g->lastMilestone           = milestoneReached;
            g->platforms.milestone     = g->lastMilestone;
            Controls_Shuffle(&g->controls);
            char msg[64];
            snprintf(msg, sizeof(msg), "[%c]=LEFT  [%c]=RIGHT",
                     g->controls.leftChar, g->controls.rightChar);
            triggerFlash(msg);
        }

        if (s_flashTimer > 0) s_flashTimer -= GetFrameTime();

        bool fellOff = Player_IsBelowScreen(&g->player, g->cameraOffsetY);
        bool hitLava = (lavaScreenY < SCREEN_HEIGHT) &&
                       (g->player.pos.y + g->cameraOffsetY + PLAYER_HEIGHT / 2.0f > lavaScreenY);

        if (fellOff || hitLava) {
            if (g->fx.haloReady) {
                g->fx.haloReady       = false;
                g->fx.haloReviveTimer = 1.5f;
                g->player.pos.y       = -g->cameraOffsetY + SCREEN_HEIGHT / 2.0f;
                g->player.vel.y       = BOUNCE_FORCE * 1.4f;
                g->fx.bootsUsed       = false;
            } else {
                if (g->score > g->highScore) g->highScore = g->score;
                g->state = STATE_GAME_OVER;
            }
        }
        break;
    }

    case STATE_GAME_OVER:
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            startGame(g);
        if (IsKeyPressed(KEY_ESCAPE))
            g->state = STATE_MENU;
        break;
    }
}

void Game_Draw(Game *g) {
    bool inverted = EventSystem_IsInverted(&g->events);

    if (inverted)
        EventSystem_BeginCapture(&g->events);
    else
        BeginDrawing();

    ClearBackground((Color){18, 18, 30, 255});

    switch (g->state) {

    case STATE_MENU: {
        int cx = SCREEN_WIDTH / 2;
        const char *title = "WHIRLYBIRD";
        int tw = MeasureText(title, 52);
        DrawText(title, cx - tw/2, 180, 52, (Color){0, 220, 140, 255});

        const char *sub = "Spin up. Don't fall.";
        int sw2 = MeasureText(sub, 20);
        DrawText(sub, cx - sw2/2, 248, 20, LIGHTGRAY);

        const char *hint = "ENTER or SPACE to play";
        int hw = MeasureText(hint, 18);
        DrawText(hint, cx - hw/2, SCREEN_HEIGHT - 120, 18, GRAY);

        const char *twist = "Keys shuffle at each milestone!";
        int tww = MeasureText(twist, 16);
        DrawText(twist, cx - tww/2, SCREEN_HEIGHT - 90, 16, (Color){255, 200, 60, 255});
        break;
    }

    case STATE_PLAYING: {
        float lavaScreenY = 0.0f;
        EventSlot *lavaSlot = &g->events.slots[EVT_LAVA];
        if (lavaSlot->active)
            lavaScreenY = lavaSlot->data + g->cameraOffsetY;
        else
            lavaScreenY = (float)SCREEN_HEIGHT * 10.0f;

        PlatformList_Draw(&g->platforms, g->cameraOffsetY);
        PowerupSystem_Draw(&g->powerups, g->cameraOffsetY);
        Player_Draw(&g->player, g->cameraOffsetY, &g->fx);

        if (lavaScreenY < (float)SCREEN_HEIGHT) {
            int ly = (int)lavaScreenY;
            if (ly < 0) ly = 0;
            DrawRectangle(0, ly, SCREEN_WIDTH, SCREEN_HEIGHT - ly,
                          Fade((Color){220, 50, 10, 255}, 0.72f));
            DrawRectangle(0, ly, SCREEN_WIDTH, 5, (Color){255, 180, 30, 255});
        }

        char scoreBuf[32];
        snprintf(scoreBuf, sizeof(scoreBuf), "HEIGHT  %d", g->score);
        DrawText(scoreBuf, 10, 10, 20, WHITE);

        int nextM = (g->lastMilestone + 1) * MILESTONE_INTERVAL;
        char nBuf[48];
        snprintf(nBuf, sizeof(nBuf), "Next shuffle: %d", nextM);
        DrawText(nBuf, 10, 34, 14, GRAY);

        if (s_flashTimer > 0) {
            float alpha = s_flashTimer / FLASH_DURATION;
            if (alpha > 1.0f) alpha = 1.0f;
            Color fc = Fade((Color){255, 220, 50, 255}, alpha);
            int fw = MeasureText(s_flashMsg, 25);
            DrawText(s_flashMsg, SCREEN_WIDTH/2 - fw/2,
                     SCREEN_HEIGHT/2 - 30, 25, fc);
        }

        if (g->fx.bootsTimer > 0.0f && !g->fx.bootsUsed) {
            const char *bh = "[SPACE] double jump";
            int bw = MeasureText(bh, 13);
            DrawText(bh, SCREEN_WIDTH/2 - bw/2, SCREEN_HEIGHT - 56,
                     13, Fade((Color){50, 220, 220, 255}, 0.8f));
        }

        Controls_DrawHUD(&g->controls);
        PowerupSystem_DrawHUD(&g->powerups, &g->fx);
        EventSystem_DrawOverlay(&g->events);

#ifdef DEBUG_BUILD
        Debug_Draw(g);
#endif
        break;
    }

    case STATE_GAME_OVER: {
        int cx = SCREEN_WIDTH / 2;
        const char *over = "GAME OVER";
        int ow = MeasureText(over, 48);
        DrawText(over, cx - ow/2, 200, 48, RED);

        char scoreLine[48], hiBuf[48];
        snprintf(scoreLine, sizeof(scoreLine), "Score:  %d", g->score);
        snprintf(hiBuf,     sizeof(hiBuf),     "Best:   %d", g->highScore);

        int slw = MeasureText(scoreLine, 24);
        int hbw = MeasureText(hiBuf, 24);
        DrawText(scoreLine, cx - slw/2, 270, 24, WHITE);
        DrawText(hiBuf,     cx - hbw/2, 300, 24, (Color){255, 200, 60, 255});

        const char *r = "ENTER / SPACE  to restart";
        int rw = MeasureText(r, 16);
        DrawText(r, cx - rw/2, SCREEN_HEIGHT - 100, 16, GRAY);

        const char *m = "ESC  for menu";
        int mw = MeasureText(m, 16);
        DrawText(m, cx - mw/2, SCREEN_HEIGHT - 76, 16, DARKGRAY);
        break;
    }
    }

    if (inverted) {
        EventSystem_EndCapture(&g->events);
        EndDrawing();
    } else {
        EndDrawing();
    }
}

void Game_Cleanup(Game *g) {
    EventSystem_Cleanup(&g->events);
}
