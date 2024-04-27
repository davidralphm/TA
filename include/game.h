#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "player.h"
#include "gamestats.h"
#include "gameglobals.h"
#include "gamesettings.h"

#include <SDL2/SDL.h>

// Tipe game yang menyimpan semua informasi dari game
typedef struct game_t {
    SDL_Window *window; // Window dari game
    SDL_Renderer *renderer; // Renderer untuk window game
    SDL_Texture *texture; // Texture yang digunakan untuk menampilkan game world
    SDL_Surface *gameTextures; // Surface yang berisi pixel dari textures yang digunakan pada game world

    unsigned *texturePixels; // Pointer ke data dari texture
    int texturePitch; // Pitch dari texture (pitch = texture width * bytes per pixel)

    GameSettings *settings;
    GameGlobals globals;
    GameStats stats;
} Game;

// Fungsi untuk menginisialisasi menu - menu game
void Game_InitMenu(void);

// Fungsi untuk mengupdate game
void Game_Update(Game *game);

// Fungsi untuk render game
void Game_Render(Game *game);

// Fungsi untuk mengatur tingkat kesulitan, mode, dan memulai game
void Game_Start(Game *game, GameDifficulty difficulty);

// Fungsi untuk menambahkan skor pemain
// Setiap kali player mendapatkan 5000 poin skor,
// jumlah lives milik player akan ditambahkan 1
//
// Fungsi ini mengembalikan jumlah lives yang didapatkan
// player berdasarkan jumlah poin yang dimiliki player
int Game_AddPlayerScore(Game *game, int score);

#endif