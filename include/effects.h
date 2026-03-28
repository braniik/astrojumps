#ifndef EFFECTS_H
#define EFFECTS_H

#include <stdbool.h>

typedef struct {
    float jetpackTimer;
    float featherTimer;
    float elixirTimer;
    float bootsTimer;
    bool haloReady;
    bool bootsUsed;
    float haloReviveTimer;
} ActiveEffects;

#endif
