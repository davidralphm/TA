#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "game.h"

// Tipe enemy
typedef struct enemy_t {
    double x, y;
    int health;

    unsigned char deleted;
    unsigned char hurt;
    
    unsigned shootTimer;

    unsigned char frame;
    unsigned animTimer;

    double targetX, targetY;
} Enemy;

// Enum untuk enemy animation frames
typedef enum enemy_frame_e {
    ENEMY_FRAME_WALK_1,
    ENEMY_FRAME_WALK_2,
    ENEMY_FRAME_WALK_3,
    ENEMY_FRAME_WALK_4,
    ENEMY_FRAME_SHOOT,
    ENEMY_FRAME_HURT,
} EnemyFrame;

// Fungsi untuk membuat enemy baru
Enemy* Enemy_New(double x, double y);

// Fungsi untuk mengupdate semua enemy dalam game
void Enemy_Update(Game *game);

// Fungsi untuk merender semua enemy dalam game
void Enemy_Render(Game *game);

// Fungsi untuk menghapus semua enemy dalam game
void Enemy_DeleteAll(void);

// Fungsi untuk get jumlah enemy
int Enemy_GetCount(void);

// Fungsi untuk get sebuah enemy
Enemy* Enemy_Get(int index);

#endif