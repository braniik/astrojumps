#ifndef GAME_H
#define GAME_H

#include "controls.h"
#include "effects.h"
#include "events.h"
#include "platform.h"
#include "player.h"
#include "powerup.h"
#include "raylib.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 720
#define TARGET_FPS 60
#define GAME_TITLE "Whirlybird"
#define MILESTONE_INTERVAL 800

typedef enum { STATE_MENU, STATE_PLAYING, STATE_GAME_OVER } GameState;

typedef struct {
    GameState state;
    Player player;
    PlatformList platforms;
    Controls controls;
    PowerupSystem powerups;
    EventSystem events;
    ActiveEffects fx;
    float cameraOffsetY;
    int score;
    int lastMilestone;
    int highScore;
    bool randomEventsEnabled;
    bool keyShuffleEnabled;
    bool bgmEnabled;
} Game;

void Game_Init(Game *g);
void Game_Update(Game *g);
void Game_Draw(Game *g);
void Game_Cleanup(Game *g);

#endif
