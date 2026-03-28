#ifndef EVENTS_H
#define EVENTS_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    EVT_LAVA = 0,
    EVT_WIND,
    EVT_FLASH,
    EVT_BLACKOUT,
    EVT_INVERSION,
    EVT_COUNT
} EventType;

typedef struct {
    bool active;
    float timer;
    float data;
} EventSlot;

typedef struct {
    EventSlot slots[EVT_COUNT];
    float nextEventTimer;
    RenderTexture2D invRT;
    bool invRTReady;
} EventSystem;

void EventSystem_Init(EventSystem *es);
void EventSystem_Reset(EventSystem *es);
void EventSystem_Cleanup(EventSystem *es);
void EventSystem_Unload(EventSystem *es);
void EventSystem_StopSounds(EventSystem *es);
void EventSystem_Update(EventSystem *es, float cameraOffsetY, float *outWindForce, float *outLavaScreenY);
bool EventSystem_IsInverted(EventSystem *es);
void EventSystem_BeginCapture(EventSystem *es);
void EventSystem_EndCapture(EventSystem *es);
void EventSystem_DrawOverlay(EventSystem *es);

#endif
