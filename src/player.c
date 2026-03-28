#include "player.h"
#include <math.h>

static Sound sfxBoots;
static Sound sfxJetpack;
static Sound sfxElixir;
static Sound sfxRevive;

void Player_Load(Player *p) {
    p->texture = LoadTexture("assets/sprites/character.png");
    p->textureJetpack = LoadTexture("assets/sprites/characterwjetpack.png");
    p->textureBoots = LoadTexture("assets/sprites/characterwboty.png");

    SetTextureFilter(p->texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(p->textureJetpack, TEXTURE_FILTER_POINT);
    SetTextureFilter(p->textureBoots, TEXTURE_FILTER_POINT);

    sfxBoots = LoadSound("assets/sfx/doublejump.mp3");
    sfxJetpack = LoadSound("assets/sfx/jetpackbeep.mp3");
    sfxElixir = LoadSound("assets/sfx/elixir.mp3");
    sfxRevive = LoadSound("assets/sfx/revive2.mp3");
}

void Player_Reset(Player *p, float startX, float startY) {
    p->pos = (Vector2){ startX, startY };
    p->vel = (Vector2){ 0.0f, 0.0f };
    p->rotation = 0.0f;
    p->alive = true;
}

void Player_Init(Player *p, float startX, float startY) {
    Player_Load(p);
    Player_Reset(p, startX, startY);
}

void Player_Update(Player *p, float moveDir, ActiveEffects *fx) {
    p->vel.x = moveDir * MOVE_SPEED;

    if (fx && fx->jetpackTimer > 0.0f) {
        p->vel.y = -8.5f;
        if (!IsSoundPlaying(sfxJetpack))
            PlaySound(sfxJetpack);
    } else {
        StopSound(sfxJetpack);
        float gravity = (fx && fx->featherTimer > 0.0f) ? GRAVITY * 0.32f : GRAVITY;
        float maxFall = (fx && fx->featherTimer > 0.0f) ? MAX_FALL_SPEED * 0.38f : MAX_FALL_SPEED;
        p->vel.y += gravity;
        if (p->vel.y > maxFall) p->vel.y = maxFall;
    }

    if (fx && fx->bootsTimer > 0.0f && !fx->bootsUsed && p->vel.y > 0.0f) {
        if (IsKeyPressed(KEY_SPACE)) {
            p->vel.y = BOUNCE_FORCE;
            fx->bootsUsed = true;
            PlaySound(sfxBoots);
        }
    }

    if (fx && fx->elixirTimer > 0.0f) {
        if (!IsSoundPlaying(sfxElixir))
            PlaySound(sfxElixir);
    } else {
        StopSound(sfxElixir);
    }

    p->pos.x += p->vel.x;
    p->pos.y += p->vel.y;

    int sw = GetScreenWidth();
    if (p->pos.x < -PLAYER_WIDTH / 2.0f) p->pos.x = (float)sw + PLAYER_WIDTH / 2.0f;
    if (p->pos.x > (float)sw + PLAYER_WIDTH / 2.0f) p->pos.x = -PLAYER_WIDTH / 2.0f;

    p->rotation = 0.0f;
}

void Player_Bounce(Player *p, ActiveEffects *fx) {
    p->vel.y = BOUNCE_FORCE;
    if (fx) fx->bootsUsed = false;
}

void Player_Draw(Player *p, float cameraOffsetY, ActiveEffects *fx) {
    float sy = p->pos.y + cameraOffsetY;
    Vector2 ctr = { p->pos.x, sy };

    Texture2D tex = p->texture;
    if (fx && fx->jetpackTimer > 0.0f)
        tex = p->textureJetpack;
    else if (fx && fx->bootsTimer > 0.0f)
        tex = p->textureBoots;

    float drawW = PLAYER_WIDTH, drawH = PLAYER_HEIGHT;
    if (fx && fx->elixirTimer > 0.0f)
        drawW = PLAYER_WIDTH * 2.5f;

    Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dest = { ctr.x, ctr.y, drawW, drawH };
    Vector2 origin = { drawW / 2.0f, drawH / 2.0f };
    DrawTexturePro(tex, source, dest, origin, p->rotation, WHITE);

    if (fx && fx->haloReviveTimer > 0.0f) {
        float alpha = fx->haloReviveTimer / 1.5f;
        if (alpha > 1.0f) alpha = 1.0f;
        DrawCircle((int)ctr.x, (int)ctr.y,
                   (int)(drawW * 1.8f), Fade((Color){255, 230, 50, 255}, alpha * 0.55f));
        if (!IsSoundPlaying(sfxRevive))
            PlaySound(sfxRevive);
    } else {
        StopSound(sfxRevive);
    }
}

bool Player_IsBelowScreen(Player *p, float cameraOffsetY) {
    return (p->pos.y + cameraOffsetY) > GetScreenHeight() + PLAYER_HEIGHT;
}

void Player_StopSounds(void) {
    StopSound(sfxBoots);
    StopSound(sfxJetpack);
    StopSound(sfxElixir);
    StopSound(sfxRevive);
}

void Player_Unload(Player *p) {
    UnloadTexture(p->texture);
    UnloadTexture(p->textureJetpack);
    UnloadTexture(p->textureBoots);
    UnloadSound(sfxBoots);
    UnloadSound(sfxJetpack);
    UnloadSound(sfxElixir);
    UnloadSound(sfxRevive);
}
