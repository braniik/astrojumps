#ifndef CONTROLS_H
#define CONTROLS_H

#include "raylib.h"
#include <stdbool.h>
#define KEY_POOL_SIZE 26

typedef struct {
    KeyboardKey leftKey;
    KeyboardKey rightKey;
    char        leftChar;
    char        rightChar;
} Controls;

void  Controls_Init(Controls *c);
void  Controls_Shuffle(Controls *c);
float Controls_GetMoveDir(Controls *c);
void  Controls_DrawHUD(Controls *c);

#endif