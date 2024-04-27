#include "enemyspawner.h"
#include "enemy.h"

#include <SDL2/SDL.h>
#include <stdlib.h>

// Jumlah maksimum enemy spawner yang diupdate
#define ENEMY_SPAWNER_UPDATE_MAX 8

// Jumlah enemy spawner maximum dalam game
#define ENEMY_SPAWNER_COUNT_MAX 256

// Array enemy spawner dalam game
static EnemySpawner ENEMY_SPAWNER[ENEMY_SPAWNER_COUNT_MAX];

// Array enemy spawner, diurutkan berdasarkan jarak terhadap pemain
static EnemySpawner* ENEMY_SPAWNER_SORTED[ENEMY_SPAWNER_COUNT_MAX];

// Jumlah enemy spawner dalam game
static unsigned ENEMY_SPAWNER_COUNT = 0;

// Fungsi untuk membandingkan jarak enemy spawner untuk sorting
static int _EnemySpawner_Compare(const void *a, const void *b) {
    EnemySpawner *spawnerA = *((EnemySpawner**) a);
    EnemySpawner *spawnerB = *((EnemySpawner**) b);

    return spawnerA->playerDistance * 64.0 - spawnerB->playerDistance * 64.0;
}

EnemySpawner* EnemySpawner_New(double x, double y, unsigned spawnInterval) {
    if (ENEMY_SPAWNER_COUNT >= ENEMY_SPAWNER_COUNT_MAX)
        return NULL;
    
    EnemySpawner *spawner = &ENEMY_SPAWNER[ENEMY_SPAWNER_COUNT++];

    spawner->x = x;
    spawner->y = y;
    spawner->spawnInterval = spawnInterval;

    spawner->spawnTimer = SDL_GetTicks() + spawner->spawnInterval;

    // Tambahkan ke dalam array untuk di sort
    ENEMY_SPAWNER_SORTED[ENEMY_SPAWNER_COUNT - 1] = spawner;

    return spawner;
}

void EnemySpawner_Update(Game *game) {
    // Update jarak terhadap pemain
    for (int i = 0; i < ENEMY_SPAWNER_COUNT; i++) {
        EnemySpawner *spawner = &ENEMY_SPAWNER[i];

        spawner->playerDistance = (spawner->x - game->globals.player.x) * (spawner->x - game->globals.player.x);
        spawner->playerDistance += (spawner->y - game->globals.player.y) * (spawner->y - game->globals.player.y);
    }

    // Sort array enemy spawner
    qsort((void*) ENEMY_SPAWNER_SORTED, ENEMY_SPAWNER_COUNT, sizeof(EnemySpawner*), _EnemySpawner_Compare);

    // Update beberapa enemy spawner terdekat ke player
    int spawnerCount = SDL_min(ENEMY_SPAWNER_COUNT, ENEMY_SPAWNER_UPDATE_MAX);

    for (int i = 0; i < spawnerCount; i++) {
        EnemySpawner *spawner = ENEMY_SPAWNER_SORTED[i];

        if (spawner->spawnTimer < SDL_GetTicks()) {
            // Buat enemy baru jika tidak ada enemy di dekat spawner
            int enemyCount = Enemy_GetCount();
            char spawn = 1;

            for (int i = 0; i < enemyCount; i++) {
                Enemy *enemy = Enemy_Get(i);

                double enemyDist = (enemy->x - spawner->x) * (enemy->x - spawner->x);
                enemyDist += (enemy->y - spawner->y) * (enemy->y - spawner->y);

                if (enemyDist <= 16.0) {
                    spawn = 0;
                    break;
                }
            }

            if (spawn)
                Enemy_New(spawner->x, spawner->y);
            
            spawner->spawnTimer = SDL_GetTicks() + spawner->spawnInterval;
        }
    }
}

void EnemySpawner_DeleteAll(void) {
    ENEMY_SPAWNER_COUNT = 0;
}
