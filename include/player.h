#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gamestats.h"

// Jumlah skor yang ditambahkan ketika player membunuh musuh
#define PLAYER_SCORE_ENEMY 100

// Jumlah skor yang ditambahkan ketika player mengambil item treasure
#define PLAYER_SCORE_TREASURE 1000

// Jumlah skor yang ditambahkan untuk tiap detik yang tersisa ketika
// player menyelesaikan level (untuk game mode escape dan pacifist)
#define PLAYER_SCORE_TIME_BONUS 10

// Angka kelipatan skor player untuk menambahkan life
#define PLAYER_LIFE_BONUS_SCORE 5000

// Jumlah health maksimum pemain
#define PLAYER_MAX_HEALTH 100

// Jumlah fireball maksimum pemain
#define PLAYER_MAX_FIREBALL 100

// Jumlah health pemain ketika game dimulai
#define PLAYER_STARTING_HEALTH 100

// Jumlah life pemain ketika game dimulai
#define PLAYER_STARTING_LIVES 3

// Jumlah fireball ketika game dimulai
#define PLAYER_STARTING_FIREBALL 20

// Speed normal
#define PLAYER_SPEED_NORMAL 1.0

// Speed jika terkena efek speed boost
#define PLAYER_SPEED_SPEED_BOOST 1.5

// Speed jika terkena efek slowness
#define PLAYER_SPEED_SLOWNESS 0.5

// Fog distance normal
#define PLAYER_FOG_DIST_NORMAL 16.0

// Fog distance jika terkena efek x - ray
#define PLAYER_FOG_DIST_XRAY 32.0

// Fog distance jika terkena efek blindness
#define PLAYER_FOG_DIST_BLINDNESS 8.0

// Tipe player
typedef struct player_t {
    double x, y;
    double angle, angleDegrees;
    double camFOV;
    double camPlaneDist;

    double speed;
    double fogDist;

    int health;
    int score;
    int lives;
    int fireball;

    char xray;

    unsigned speedTimer;
    unsigned visionTimer;

    char mouseClick, lastClick;
    char isPacifist;
} Player;

// Fungsi untuk menginisialisasi player
void Player_Init(Player *player, double x, double y, double angle, double fov);

// Fungsi untuk memposisikan player dan menghapus efek dari item
// Health, lives, score, dan jumlah fireball tidak berubah
void Player_SetPosition(Player *player);

// Fungsi untuk reset semua stats player
void Player_ResetStats(Player *player);

// Fungsi untuk mengupdate player
void Player_Update(Player *player, double damageMultiplier, GameStats *gameStats);

#endif