#ifndef __ENEMY_SPAWNER_H__
#define __ENEMY_SPAWNER_H__

#include "game.h"

// Default enemy spawner delay
#define ENEMY_SPAWNER_DELAY_DEFAULT 5000

// Tipe enemy spawner
typedef struct enemy_spawner_t {
    double x, y;
    double playerDistance;
    unsigned spawnInterval, spawnTimer;
} EnemySpawner;

// Fungsi untuk membuat enemy spawner baru
EnemySpawner* EnemySpawner_New(double x, double y, unsigned spawnInterval);

// Fungsi untuk mengupdate semua enemy spawner dalam game
void EnemySpawner_Update(Game *game);

// Fungsi untuk menghapus semua enemy spawner
void EnemySpawner_DeleteAll(void);

#endif