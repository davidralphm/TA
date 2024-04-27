#include "enemy.h"
#include "sprite.h"
#include "map.h"
#include "fireball.h"
#include "textures.h"
#include "sounds.h"
#include "raycaster.h"
#include "item.h"

#include <math.h>

// Pengali kecepatan pergerakan musuh
#define ENEMY_SPEED_MUL 0.025

// Besar collision offset untuk collision checking
#define ENEMY_COLL_OFFSET 0.2

// Jumlah maksimum enemy dalam game
#define ENEMY_COUNT_MAX 32

// Jarak minimum enemy ke target pergerakannya
#define ENEMY_MIN_DIST_TARGET 0.0

// Jarak minimum enemy ke pemain untuk mulai menembaki pemain
#define ENEMY_MIN_DIST_PLAYER 36.0

// Array enemy dalam game
static Enemy ENEMY[ENEMY_COUNT_MAX];

// Jumlah enemy dalam game
static unsigned ENEMY_COUNT = 0;

// Fungsi untuk mengupdate sebuah enemy
static void _Enemy_Update(Game *game, Enemy *enemy) {
    // Jika health enemy <= 0, hapus enemy
    if (enemy->health <= 0) {
        enemy->deleted = 1;

        // Enemy akan drop fireball untuk player
        Item_New(enemy->x, enemy->y, ITEM_TYPE_FIREBALL);

        game->globals.player.lives += Game_AddPlayerScore(game, PLAYER_SCORE_ENEMY);

        // Untuk stats
        game->stats.scoreGot += PLAYER_SCORE_ENEMY;
        game->stats.totalScoreGot += PLAYER_SCORE_ENEMY;

        game->stats.enemiesKilled++;
        game->stats.totalEnemiesKilled++;
        
        return;
    }

    // Jika enemy terkena fireball milik player, kurangi health enemy
    Fireball_CheckEnemyHit(enemy, &game->stats);

    // Jika enemy terkena fireball player, enemy akan menampilkan frame
    // animasi terluka dan tidak dapat bergerak atau menembak
    if (enemy->hurt) {
        if (enemy->animTimer < SDL_GetTicks()) {
            enemy->hurt = 0;
        }

        enemy->frame = ENEMY_FRAME_HURT;
        enemy->shootTimer = SDL_GetTicks() + 1000;
        return;
    }

    // Hitung jarak enemy ke player
    double distToPlayer = (game->globals.player.x - enemy->x) * (game->globals.player.x - enemy->x);
    distToPlayer += (game->globals.player.y - enemy->y) * (game->globals.player.y - enemy->y);

    // Hitung direction ke arah player
    double dirToPlayer = atan2(game->globals.player.y - enemy->y, game->globals.player.x - enemy->x);

    // Jika enemy dapat melihat player, set target position
    // enemy ke posisi player
    char playerVisible = 0;

    double rayDist = Raycaster_CastRay(enemy->x, enemy->y, dirToPlayer);
    rayDist *= rayDist;

    if (rayDist >= distToPlayer) {
        enemy->targetX = game->globals.player.x;
        enemy->targetY = game->globals.player.y;

        playerVisible = 1;
    }
    
    // Gerakkan enemy ke arah player jika enemy berada cukup
    // jauh dari player
    double distToTarget = (enemy->targetX - enemy->x) * (enemy->targetX - enemy->x);
    distToTarget += (enemy->targetY - enemy->y) * (enemy->targetY - enemy->y);

    double minDist = (!playerVisible) ? ENEMY_MIN_DIST_TARGET : ENEMY_MIN_DIST_PLAYER;

    if (distToTarget > minDist) {
        double dirToTarget = atan2(enemy->targetY - enemy->y, enemy->targetX - enemy->x);

        double dirX = cos(dirToTarget);
        double dirY = sin(dirToTarget);

        double newX = enemy->x + dirX * ENEMY_SPEED_MUL;
        double newY = enemy->y + dirY * ENEMY_SPEED_MUL;

        double collisionDistanceX = (dirX < 0.0) ? -ENEMY_COLL_OFFSET : ENEMY_COLL_OFFSET;
        double collisionDistanceY = (dirY < 0.0) ? -ENEMY_COLL_OFFSET : ENEMY_COLL_OFFSET;

        // X - axis collision detection
        if (
            Map_Get((unsigned) (newX + collisionDistanceX), (unsigned) (enemy->y - ENEMY_COLL_OFFSET))->isObstacle == 0 &&
            Map_Get((unsigned) (newX + collisionDistanceX), (unsigned) (enemy->y + ENEMY_COLL_OFFSET))->isObstacle == 0
        )
            enemy->x = newX;
        
        // Y - axis collision detection
        if (
            Map_Get((unsigned) (enemy->x - ENEMY_COLL_OFFSET), (unsigned) (newY + collisionDistanceY))->isObstacle == 0 &&
            Map_Get((unsigned) (enemy->x + ENEMY_COLL_OFFSET), (unsigned) (newY + collisionDistanceY))->isObstacle == 0
        )
            enemy->y = newY;

        // Reset timer
        enemy->shootTimer = SDL_GetTicks() + 1000;

        // Animasi berjalan musuh
        if (enemy->animTimer < SDL_GetTicks()) {
            enemy->animTimer = SDL_GetTicks() + 500;
            enemy->frame = (enemy->frame + 1) & 3;
        }
    } else {
        // Jika enemy berada dekat dengan player, enemy akan menembakkan fireball
        // dengan delay tertentu

        if (enemy->shootTimer < SDL_GetTicks()) {
            if (playerVisible) {
                double dirX = cos(dirToPlayer);
                double dirY = sin(dirToPlayer);

                Fireball_New(enemy->x, enemy->y, dirX, dirY, 1, &game->stats);
                Sounds_Play3D(SOUND_EFFECT_FIREBALL, enemy->x, enemy->y, &game->globals.player);
            }
            enemy->shootTimer = SDL_GetTicks() + 1000;
        }

        // Set frame animasi musuh
        enemy->frame = ENEMY_FRAME_SHOOT;
    }
}

// Fungsi untuk menghapus sebuah enemy
void _Enemy_Free(Enemy *enemy) {
    unsigned index = enemy - ENEMY;

    if (index >= ENEMY_COUNT) return;

    ENEMY[index] = ENEMY[--ENEMY_COUNT];
}

Enemy* Enemy_New(double x, double y) {
    if (ENEMY_COUNT >= ENEMY_COUNT_MAX) return NULL;

    Enemy *enemy = &ENEMY[ENEMY_COUNT++];

    enemy->x = x;
    enemy->y = y;
    enemy->health = 100;
    enemy->deleted = 0;
    enemy->shootTimer = SDL_GetTicks() + 1000;

    enemy->targetX = enemy->x;
    enemy->targetY = enemy->y;

    return enemy;
}

void Enemy_Update(Game *game) {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        _Enemy_Update(game, &ENEMY[i]);
    }

    // Hapus enemy
    for (int i = 0; i < ENEMY_COUNT; i++)
        if (ENEMY[i].deleted)
            _Enemy_Free(&ENEMY[i--]);
}

void Enemy_Render(Game *game) {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        Enemy *enemy = &ENEMY[i];

        Sprite_Add(&game->globals.player, enemy->x, enemy->y, TEXTURE_ID_ENEMY_FRAME_1 + enemy->frame, 1.0);
    }
}

void Enemy_DeleteAll(void) {
    ENEMY_COUNT = 0;
}

int Enemy_GetCount(void) {
    return ENEMY_COUNT;
}

Enemy* Enemy_Get(int index) {
    if (index < 0 || index >= ENEMY_COUNT)
        return NULL;
    
    return &ENEMY[index];
}