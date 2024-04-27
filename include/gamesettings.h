#ifndef __GAME_SETTINGS_H__
#define __GAME_SETTINGS_H__

#include "game.h"

typedef struct game_settings_t {
    unsigned char mazeSize; // Variabel yang menyimpan ukuran labiran untuk tingkat kesulitan sekarang

    unsigned timeLimit; // Variabel yang menyimpan batas waktu game (dalam detik)
    unsigned survivalTimeLimit; // Variabel yang menyimpan batas waktu game mode survival (dalam detik)

    double trapSpeed; // Variabel yang menyimpan pengali kecepatan item jebakan
    double enemyFireballDamage; // Variabel yang menyimpan pengali damage fireball musuh
} GameSettings;

#endif