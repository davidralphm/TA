#include "fireball.h"
#include "map.h"
#include "sprite.h"
#include "game.h"
#include "enemy.h"
#include "textures.h"
#include "sounds.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>

// Pengali kecepatan pergerakan fireball dalam game
#define FIREBALL_SPEED_MUL 0.15

// Jarak untuk fireball hit pemain atau musuh
#define FIREBALL_HIT_DIST 0.8

// Base damage fireball ketika mengenai pemain
#define FIREBALL_BASE_DAMAGE_PLAYER 5

// Base damage fireball ketika mengenai musuh
#define FIREBALL_BASE_DAMAGE_ENEMY 15

// Jumlah random damage fireball
#define FIREBALL_DAMAGE_RANDOM 6

// Jumlah maksimum fireball dalam game
#define FIREBALL_COUNT_MAX 256

// Array fireball dalam game
static Fireball FIREBALL[FIREBALL_COUNT_MAX];

// Jumlah fireball dalam game
static unsigned FIREBALL_COUNT = 0;

// Fungsi untuk mengupdate sebuah fireball
static void _Fireball_Update(Fireball *fireball) {
    fireball->x += fireball->dirX * FIREBALL_SPEED_MUL;
    fireball->y += fireball->dirY * FIREBALL_SPEED_MUL;

    // Jika fireball mengenai tembok, hapus fireball
    if (Map_Get((unsigned) fireball->x, (unsigned) fireball->y)->isWall)
        fireball->deleted = 1;
}

// Fungsi untuk menghapus sebuah fireball
void _Fireball_Free(Fireball *fireball) {
    // Hitung index fireball dalam array
    unsigned index = SDL_clamp(fireball - FIREBALL, 0, FIREBALL_COUNT_MAX - 1);

    FIREBALL[index] = FIREBALL[--FIREBALL_COUNT];
}

Fireball* Fireball_New(
    double x, double y,
    double dirX, double dirY,
    unsigned char enemyFireball,
    GameStats *gameStats
) {
    if (FIREBALL_COUNT >= FIREBALL_COUNT_MAX)
        return NULL;
    
    Fireball *fireball = &FIREBALL[FIREBALL_COUNT++];

    fireball->x = x;
    fireball->y = y;
    fireball->dirX = dirX;
    fireball->dirY = dirY;
    fireball->enemyFireball = enemyFireball;
    fireball->deleted = 0;

    // Untuk stats
    if (!fireball->enemyFireball) {
        gameStats->fireballsShot++;
        gameStats->totalFireballsShot++;
    }

    return fireball;
}

void Fireball_Update(Player *player) {
    // Gerakkan semua fireball dalam game
    for (int i = 0; i < FIREBALL_COUNT; i++)
        _Fireball_Update(&FIREBALL[i]);
    
    // Hapus semua fireball yang mengenai tembok
    for (int i = 0; i < FIREBALL_COUNT; i++)
        if (FIREBALL[i].deleted) {
            // Play sound effect explosion fireball
            Sounds_Play3D(SOUND_EFFECT_EXPLOSION, FIREBALL[i].x, FIREBALL[i].y, player);

            _Fireball_Free(&FIREBALL[i--]);
        }
}

void Fireball_CheckPlayerHit(Player *player, double damageMultiplier, GameStats *gameStats) {
    for (int i = 0; i < FIREBALL_COUNT; i++) {
        Fireball *fireball = &FIREBALL[i];

        if (fireball->enemyFireball == 0 || fireball->deleted) continue;

        double dist = (fireball->x - player->x) * (fireball->x - player->x);
        dist += (fireball->y - player->y) * (fireball->y - player->y);

        if (dist <= FIREBALL_HIT_DIST) {
            fireball->deleted = 1;

            int damage = (FIREBALL_BASE_DAMAGE_PLAYER + (rand() % FIREBALL_DAMAGE_RANDOM)) * damageMultiplier;
            player->health -= damage;

            // Untuk stats
            gameStats->damageReceived += damage;
            gameStats->totalDamageReceived += damage;
        }
    }
}

void Fireball_CheckEnemyHit(Enemy *enemy, GameStats *gameStats) {
    for (int i = 0; i < FIREBALL_COUNT; i++) {
        Fireball *fireball = &FIREBALL[i];

        if (fireball->enemyFireball == 1 || fireball->deleted) continue;

        double dist = (fireball->x - enemy->x) * (fireball->x - enemy->x);
        dist += (fireball->y - enemy->y) * (fireball->y - enemy->y);

        if (dist <= FIREBALL_HIT_DIST) {
            fireball->deleted = 1;

            int damage = FIREBALL_BASE_DAMAGE_ENEMY + (rand() % FIREBALL_DAMAGE_RANDOM);
            enemy->health -= damage;
            enemy->hurt = 1;
            enemy->animTimer = SDL_GetTicks() + 500;

            // Untuk stats
            gameStats->damageDealt += damage;
            gameStats->totalDamageDealt += damage;
        }
    }
}

void Fireball_Render(Player *player) {
    for (int i = 0; i < FIREBALL_COUNT; i++) {
        Fireball *fireball = &FIREBALL[i];

        Sprite_Add(player, fireball->x, fireball->y, TEXTURE_ID_FIREBALL_FRAME_1 + ((SDL_GetTicks() >> 7) % 4), 1.5);
    }
}

void Fireball_DeleteAll(void) {
    FIREBALL_COUNT = 0;
}
