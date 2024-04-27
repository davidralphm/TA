#ifndef __FIREBALL_H__
#define __FIREBALL_H__

#include "game.h"
#include "gamestats.h"
#include "enemy.h"

// Tipe fireball
typedef struct fireball_t {
    double x, y;
    double dirX, dirY;

    unsigned char enemyFireball:1,
                  deleted:1;
} Fireball;

// Fungsi untuk membuat fireball baru
Fireball* Fireball_New(double x, double y, double dirX, double dirY, unsigned char enemyFireball, GameStats *gameStats);

// Fungsi untuk mengupdate semua fireball dalam game
void Fireball_Update(Player *player);

// Fungsi untuk mengecek jika player terkena fireball
void Fireball_CheckPlayerHit(Player *player, double damageMultiplier, GameStats *gameStats);

// Fungsi untuk mengecek jika enemy terkena fireball
void Fireball_CheckEnemyHit(Enemy *enemy, GameStats *gameStats);

// Fungsi untuk merender semua fireball dalam game
void Fireball_Render(Player *player);

// Fungsi untuk menghapus semua fireball
void Fireball_DeleteAll(void);

#endif