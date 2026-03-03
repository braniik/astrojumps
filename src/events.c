#include "events.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>

#define LAVA_DURATION 30.0f
#define LAVA_RISE_SPEED 1.8f
#define WIND_DURATION 5.0f
#define WIND_FORCE 1.6f
#define FLASH_DURATION 5.0f
#define FLASH_SUBINTERVAL 1.0f
#define BLACKOUT_DURATION 15.0f
#define BLACKOUT_RAMP 4.0f
#define BLACKOUT_HOLD 3.0f
#define INVERSION_DURATION 20.0f
#define EVENT_INTERVAL 60.0f

static void triggerRandom(EventSystem *es, float cameraOffsetY) {
    int tries = 0;
    EventType t;
    do {
        t = (EventType)(rand() % EVT_COUNT);
        tries++;
    } while (es->slots[t].active && tries < EVT_COUNT * 3);

    EventSlot *s = &es->slots[t];
    s->active = true;

    switch (t) {
        case EVT_LAVA:
            s->timer = LAVA_DURATION;
            s->data  = -cameraOffsetY + (float)SCREEN_HEIGHT + 50.0f;
            break;
        case EVT_WIND:
            s->timer = WIND_DURATION;
            s->data  = (rand() % 2) ? 1.0f : -1.0f;
            break;
        case EVT_FLASH:
            s->timer = FLASH_DURATION;
            s->data  = FLASH_SUBINTERVAL;
            break;
        case EVT_BLACKOUT:
            s->timer = BLACKOUT_DURATION;
            s->data  = 0.0f;
            break;
        case EVT_INVERSION:
            s->timer = INVERSION_DURATION;
            break;
        default: break;
    }
}

void EventSystem_Init(EventSystem *es) {
    for (int i = 0; i < EVT_COUNT; i++) es->slots[i] = (EventSlot){0};
    es->nextEventTimer = EVENT_INTERVAL;
    es->invRT      = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    es->invRTReady = true;
}

void EventSystem_Reset(EventSystem *es) {
    for (int i = 0; i < EVT_COUNT; i++) es->slots[i] = (EventSlot){0};
    es->nextEventTimer = EVENT_INTERVAL;
}

void EventSystem_Cleanup(EventSystem *es) {
    if (es->invRTReady) {
        UnloadRenderTexture(es->invRT);
        es->invRTReady = false;
    }
}

void EventSystem_Update(EventSystem *es, float cameraOffsetY, float *outWindForce, float *outLavaScreenY) {
    float dt = GetFrameTime();
    *outWindForce   = 0.0f;
    *outLavaScreenY = (float)SCREEN_HEIGHT * 10.0f;

    es->nextEventTimer -= dt;
    if (es->nextEventTimer <= 0.0f) {
        es->nextEventTimer = EVENT_INTERVAL;
        triggerRandom(es, cameraOffsetY);
    }

    {
        EventSlot *s = &es->slots[EVT_LAVA];
        if (s->active) {
            s->timer -= dt;
            s->data  -= LAVA_RISE_SPEED;
            *outLavaScreenY = s->data + cameraOffsetY;
            if (s->timer <= 0.0f) s->active = false;
        }
    }

    {
        EventSlot *s = &es->slots[EVT_WIND];
        if (s->active) {
            s->timer -= dt;
            *outWindForce = s->data * WIND_FORCE;
            if (s->timer <= 0.0f) s->active = false;
        }
    }

    {
        EventSlot *s = &es->slots[EVT_FLASH];
        if (s->active) {
            s->timer -= dt;
            s->data  -= dt;
            if (s->timer <= 0.0f) s->active = false;
        }
    }

    {
        EventSlot *s = &es->slots[EVT_BLACKOUT];
        if (s->active) {
            s->timer -= dt;
            float elapsed = BLACKOUT_DURATION - s->timer;
            float alpha;
            if      (elapsed < BLACKOUT_RAMP)
                alpha = elapsed / BLACKOUT_RAMP;
            else if (elapsed < BLACKOUT_RAMP + BLACKOUT_HOLD)
                alpha = 1.0f;
            else {
                float t2 = elapsed - BLACKOUT_RAMP - BLACKOUT_HOLD;
                alpha = 1.0f - t2 / BLACKOUT_RAMP;
                if (alpha < 0.0f) alpha = 0.0f;
            }
            s->data = alpha;
            if (s->timer <= 0.0f) s->active = false;
        }
    }

    {
        EventSlot *s = &es->slots[EVT_INVERSION];
        if (s->active) {
            s->timer -= dt;
            if (s->timer <= 0.0f) s->active = false;
        }
    }
}

bool EventSystem_IsInverted(EventSystem *es) {
    return es->slots[EVT_INVERSION].active;
}

void EventSystem_BeginCapture(EventSystem *es) {
    BeginTextureMode(es->invRT);
}

void EventSystem_EndCapture(EventSystem *es) {
    EndTextureMode();
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(
        es->invRT.texture,
        (Rectangle){0, 0, (float)SCREEN_WIDTH, -(float)SCREEN_HEIGHT},
        (Rectangle){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
        (Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f},
        180.0f,
        WHITE
    );
}

void EventSystem_DrawOverlay(EventSystem *es) {
    {
        EventSlot *s = &es->slots[EVT_WIND];
        if (s->active) {
            float alpha = s->timer / WIND_DURATION;
            const char *arrow = (s->data > 0) ? ">>> WIND >>>" : "<<< WIND <<<";
            int tw = MeasureText(arrow, 20);
            DrawText(arrow, SCREEN_WIDTH/2 - tw/2, SCREEN_HEIGHT/2 - 60,
                     20, Fade((Color){100, 200, 255, 255}, alpha * 0.75f));
        }
    }

    {
        EventSlot *s = &es->slots[EVT_FLASH];
        if (s->active && s->data <= 0.0f) {
            s->data = FLASH_SUBINTERVAL + (float)(rand() % 60) / 100.0f;
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(WHITE, 0.88f));
        }
    }

    {
        EventSlot *s = &es->slots[EVT_BLACKOUT];
        if (s->active && s->data > 0.0f)
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, s->data));
    }

    static const char *EVT_NAMES[EVT_COUNT]      = { "RISING LAVA","WIND","FLASH","BLACKOUT","INVERSION" };
    static const Color  EVT_COLORS[EVT_COUNT]     = {
        {255, 60,  20, 255}, {100,200,255,255},
        {255,255,200,255},   {150,150,150,255}, {180,100,255,255}
    };
    int ty = 10;
    for (int i = 0; i < EVT_COUNT; i++) {
        if (!es->slots[i].active) continue;
        int tw = MeasureText(EVT_NAMES[i], 13);
        DrawText(EVT_NAMES[i], SCREEN_WIDTH/2 - tw/2, ty, 13, EVT_COLORS[i]);
        ty += 16;
    }
}
