#ifndef __GAME_GLOBALS_H__
#define __GAME_GLOBALS_H__

#include "player.h"

// Enum halaman game
typedef enum game_page_e {
    GAME_PAGE_MAIN_MENU,
    GAME_PAGE_DIFFICULTY,
    GAME_PAGE_HELP,
    GAME_PAGE_MAIN,
    GAME_PAGE_PAUSE,
    GAME_PAGE_WIN_LEVEL,
    GAME_PAGE_LOSE_LEVEL,
    GAME_PAGE_WIN_GAME,
    GAME_PAGE_LOSE_GAME,
} GamePage;

// Enum tingkat kesulitan game
typedef enum game_diffculty_e {
    GAME_DIFFICULTY_EASY,
    GAME_DIFFICULTY_MEDIUM,
    GAME_DIFFICULTY_HARD,
} GameDifficulty;

// Enum mode game
typedef enum game_mode_e {
    GAME_MODE_ESCAPE,
    GAME_MODE_SURVIVAL,
    GAME_MODE_PACIFIST,
} GameMode;

typedef struct game_globals_t {
    GamePage page; // Variabel yang menyimpan game page sekarang
    GameDifficulty difficulty; // Variabel yang menyimpan tingkat kesulitan game yang sekarang
    GameMode mode; // Variabel yang menyimpan mode game yang sekarang
    
    Player player; // Variabel pemain

    unsigned char currentLevel; // Level game yang sekarang
    unsigned char helpPage; // Halaman help menu

    unsigned timeLeft; // Variabel yang menyimpan sisa waktu
    unsigned lastTicks; // Variabel yang digunakan untuk mengupdate time left

    unsigned char minimap; // 0 - Minimap tidak ditampilkan, 1 - Minimap ditampilkan
} GameGlobals;

#endif