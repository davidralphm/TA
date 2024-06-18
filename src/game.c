#include "game.h"
#include "font.h"
#include "button.h"
#include "item.h"
#include "sprite.h"
#include "raycaster.h"
#include "map.h"
#include "fireball.h"
#include "enemy.h"
#include "enemyspawner.h"
#include "decoration.h"
#include "ui.h"
#include "minimap.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

// Lebar button yang digunakan dalam tiap menu
#define GAME_MENU_BUTTON_W 128

// Tinggi button yang digunakan dalam tiap menu
#define GAME_MENU_BUTTON_H 32

// Lebar button yang digunakan untuk title tiap menu
#define GAME_MENU_TITLE_W 256

// Tinggi button yang digunakan untuk title tiap menu
#define GAME_MENU_TITLE_H 32

// Posisi untuk title menu
#define GAME_MENU_TITLE_Y 32

// Jumlah level yang digunakan dalam game
#define GAME_LEVEL_COUNT 5

// Level pertama dari game
#define GAME_FIRST_LEVEL 1

// Game settings untuk difficulty easy
static GameSettings GAME_SETTINGS_EASY = {
    16,                 // Ukuran labirin
    3 * 60,             // Batas waktu game mode escape dan pacifist
    2 * 60,           // Batas waktu game mode survival
    1.0,                // Kecepatan item trap
    1.0,                // Damage fireball musuh
};

// Game settings untuk difficulty medium
static GameSettings GAME_SETTINGS_MEDIUM = {
    24,                 // Ukuran labirin
    6 * 60,             // Batas waktu game mode escape dan pacifist
    4 * 60,             // Batas waktu game mode survival
    1.25,               // Kecepatan item trap
    1.5,                // Damage fireball musuh
};

// Game settings untuk difficulty hard
static GameSettings GAME_SETTINGS_HARD = {
    32,                 // Ukuran labirin
    10 * 60,            // Batas waktu game mode escape dan pacifist
    5 * 60,             // Batas waktu game mode survival
    1.5,                // Kecepatan item trap
    2.0,                // Damage fireball musuh
};

// Isi dari help menu halaman 1
static const char *HELP_MENU_LINES_1[] = {
    "Game modes :",
    "\n",
    "Escape   - Escape from the maze before time runs out",
    "Survival - Survive the endless waves of enemy and",
    "           exit the maze when time runs out",
    "Pacifist - Escape from the maze before time runs out",
    "           without attacking enemies",
    "\n",
    "Controls :",
    "\n",
    "W, A, S, D         - Player movement",
    "Mouse movement     - Player rotation",
    "Left mouse button  - Shoot fireball",
    "~                  - Pause game",
    "M                  - Show minimap",
    "\0"
};

// Isi dari help menu halaman 2
static const char *HELP_MENU_LINES_2[] = {
    "Items and powerups :",
    "\n",
    "Treasure    : Gives the player 1000 points",
    "Health      : Restore 20 health points",
    "Life        : Adds one life",
    "Time Bonus  : Adds 30 seconds of time for escape and",
    "              pacifist game modes, and speeds up the",
    "              time by 30 seconds for survival game mode",
    "Speed Boost : Speeds up player movement for 30 seconds",
    "X - Ray     : Allows the player to see items and enemies",
    "              through walls for 30 seconds",
    "\0"
};

// Isi dari help menu halaman 3
static const char *HELP_MENU_LINES_3[] = {
    "Traps :",
    "\n",
    "Slowness  : Slows down player movement for 15 seconds",
    "Blindness : Limits player visibility for 20 seconds",
    "Teleport  : Teleports the player to a random location in",
    "            the maze",
    "\n",
    "Scoring :",
    "\n",
    "Player will get 100 points every time they kill an enemy.",
    "Player will get a bonus life for every 5000 points got.",
    "Player will be given 10 bonus points for every second of",
    "remaining time left after finishing a level in the escape",
    "or pacifist game mode.",
    "\0"
};

// Array untuk isi halaman - halaman help menu
static const char **HELP_MENU_LINES[] = {
    HELP_MENU_LINES_1,
    HELP_MENU_LINES_2,
    HELP_MENU_LINES_3,
};

//Button yang digunakan dalam main menu

static Button BUTTON_MAIN_MENU_START,
              BUTTON_MAIN_MENU_HELP,
              BUTTON_MAIN_MENU_EXIT;

// Button yang digunakan dalam difficulty selection menu

static Button BUTTON_DIFFICULTY_EASY,
              BUTTON_DIFFICULTY_MEDIUM,
              BUTTON_DIFFICULTY_HARD;
            
// Button yang digunakan ketika pemain memenangkan level
static Button BUTTON_WIN_LEVEL_CONTINUE;

// Button yang digunakan ketika pemain gagal memenangkan level
static Button BUTTON_LOSE_LEVEL_RESTART;

// Button yang digunakan ketika pemain memenangkan atau kalah game
static Button BUTTON_GAME_PLAY_AGAIN;

// Button untuk resume game
static Button BUTTON_RESUME;

// Button untuk kembali ke main - menu
static Button BUTTON_MAIN_MENU;

// Button yang digunakan dalam help menu

static Button BUTTON_HELP_MENU_NEXT,
              BUTTON_HELP_MENU_PREVIOUS;

// Title untuk tiap menu
static Button TITLE_MAIN_MENU,
              TITLE_DIFFICULTY,
              TITLE_HELP,
              TITLE_PAUSE,
              TITLE_WIN_LEVEL,
              TITLE_WIN_GAME,
              TITLE_LOSE_LEVEL,
              TITLE_LOSE_GAME;

// Button yang digunakan pada main menu
static Button* MAIN_MENU_BUTTONS[] = {
    &TITLE_MAIN_MENU,
    &BUTTON_MAIN_MENU_START,
    &BUTTON_MAIN_MENU_HELP,
    &BUTTON_MAIN_MENU_EXIT,
    NULL
};

// Button yang digunakan pada difficulty selection menu
static Button *DIFFICULTY_MENU_BUTTONS[] = {
    &TITLE_DIFFICULTY,
    &BUTTON_DIFFICULTY_EASY,
    &BUTTON_DIFFICULTY_MEDIUM,
    &BUTTON_DIFFICULTY_HARD,
    &BUTTON_MAIN_MENU,
    NULL
};

// Button yang digunakan pada help menu
static Button *HELP_MENU_BUTTONS[] = {
    &TITLE_HELP,
    &BUTTON_HELP_MENU_NEXT,
    &BUTTON_HELP_MENU_PREVIOUS,
    &BUTTON_MAIN_MENU,
    NULL
};

// Button yang digunakan pada pause menu
static Button *PAUSE_MENU_BUTTONS[] = {
    &TITLE_PAUSE,
    &BUTTON_RESUME,
    &BUTTON_MAIN_MENU,
    NULL
};

// Button yang digunakan pada win level menu
static Button *WIN_LEVEL_BUTTONS[] = {
    &TITLE_WIN_LEVEL,
    &BUTTON_WIN_LEVEL_CONTINUE,
    &BUTTON_MAIN_MENU,
    NULL
};

// Button yang digunakan pada lose level menu
static Button *LOSE_LEVEL_BUTTONS[] = {
    &TITLE_LOSE_LEVEL,
    &BUTTON_LOSE_LEVEL_RESTART,
    &BUTTON_MAIN_MENU,
    NULL
};

// Button yang digunakan pada win game menu
static Button *WIN_GAME_BUTTONS[] = {
    &TITLE_WIN_GAME,
    &BUTTON_GAME_PLAY_AGAIN,
    &BUTTON_MAIN_MENU,
    NULL
};

// Button yang digunakan pada lose game menu
static Button *LOSE_GAME_BUTTONS[] = {
    &TITLE_LOSE_GAME,
    &BUTTON_GAME_PLAY_AGAIN,
    &BUTTON_MAIN_MENU,
    NULL
};

// Pointer untuk array button yang digunakan pada suatu menu
static Button** MENU_BUTTONS[] = {
    MAIN_MENU_BUTTONS,
    DIFFICULTY_MENU_BUTTONS,
    HELP_MENU_BUTTONS,
    PAUSE_MENU_BUTTONS,
    PAUSE_MENU_BUTTONS,
    WIN_LEVEL_BUTTONS,
    LOSE_LEVEL_BUTTONS,
    WIN_GAME_BUTTONS,
    LOSE_GAME_BUTTONS
};

// Fungsi callback untuk BUTTON_MAIN_MENU_START
static void _Game_ButtonMainMenuStart(Game *game) {
    game->globals.page = GAME_PAGE_DIFFICULTY;
}

// Fungsi callback untuk BUTTON_MAIN_MENU_HELP
static void _Game_ButtonMainMenuHelp(Game *game) {
    game->globals.page = GAME_PAGE_HELP;
}

// Fungsi callback untuk BUTTON_MAIN_MENU_EXIT
static void _Game_ButtonMainMenuExit(Game *game) {
    SDL_Event quitEvent;
    quitEvent.type = SDL_QUIT;

    SDL_PushEvent(&quitEvent);
}

// Fungsi callback untuk BUTTON_DIFFICULTY_EASY
static void _Game_ButtonDifficultyEasy(Game *game) {
    Game_Start(game, GAME_DIFFICULTY_EASY);
}

// Fungsi callback untuk BUTTON_DIFFICULTY_MEDIUM
static void _Game_ButtonDifficultyMedium(Game *game) {
    Game_Start(game, GAME_DIFFICULTY_MEDIUM);
}

// Fungsi callback untuk BUTTON_DIFFICULTY_HARD
static void _Game_ButtonDifficultyHard(Game *game) {
    Game_Start(game, GAME_DIFFICULTY_HARD);
}

// Fungsi callback untuk BUTTON_MAIN_MENU
static void _Game_ButtonMainMenu(Game *game) {
    game->globals.page = GAME_PAGE_MAIN_MENU;

    // Reset help page
    game->globals.helpPage = 0;

    // Reset stats player
    Player_ResetStats(&game->globals.player);

    // Reset level
    game->globals.currentLevel = 1;
}

// Fungsi callback untuk BUTTON_HELP_MENU_NEXT
static void _Game_ButtonHelpMenuNext(Game *game) {
    game->globals.helpPage = SDL_clamp(game->globals.helpPage + 1, 0, 2);
}

// Fungsi callback untuk BUTTON_HELP_MENU_PREVIOUS
static void _Game_ButtonHelpMenuPrevious(Game *game) {
    game->globals.helpPage = SDL_clamp(game->globals.helpPage - 1, 0, 2);
}

// Fungsi callback untuk BUTTON_WIN_LEVEL_CONTINUE dan BUTTON_LOSE_LEVEL_RESTART
static void _Game_ButtonNextLevelRestart(Game *game) {
    // Jika player memenangkan level, tambahkan 10 poin untuk setiap
    // sisa waktu yang dimiliki player (untuk game mode escape dan pacifist)
    if (game->globals.page == GAME_PAGE_WIN_LEVEL && game->globals.mode != GAME_MODE_SURVIVAL) {
        int addedScore = game->globals.timeLeft * PLAYER_SCORE_TIME_BONUS;

        game->globals.player.lives += Game_AddPlayerScore(game, addedScore);

        // Untuk stats
        game->stats.totalScoreGot += addedScore;
    }

    Game_Start(game, game->globals.difficulty);
}

// Fungsi callback untuk BUTTON_RESUME
static void _Game_ButtonResume(Game *game) { game->globals.page = GAME_PAGE_MAIN; }

// Fungsi untuk mengupdate menu
static void _Game_UpdateMenu(Game *game) {
    int mouseX, mouseY;
    SDL_BUTTON(SDL_GetMouseState(&mouseX, &mouseY));

    for (int i = 0; ; i++) {
        Button *button = MENU_BUTTONS[game->globals.page][i];

        if (button == NULL)
            break;
        
        Button_Update(game, button, mouseX, mouseY, game->globals.player.mouseClick);
    }
}

// Fungsi untuk mengupdate main game
static void _Game_UpdateMain(Game *game) {
    Player_Update(&game->globals.player, game->settings->enemyFireballDamage, &game->stats);
    Item_Update(game);
    Fireball_Update(&game->globals.player);
    EnemySpawner_Update(game);
    Enemy_Update(game);

    // Cek jika player berada di exit cell
    if (Map_Get((unsigned )game->globals.player.x, (unsigned) game->globals.player.y)->isExit) {
        // Jika game mode bukan survival, maka player menang
        if (game->globals.mode != GAME_MODE_SURVIVAL) {
            if (game->globals.currentLevel < GAME_LEVEL_COUNT) {
                game->globals.page = GAME_PAGE_WIN_LEVEL;
                game->globals.currentLevel++;
            } else {
                game->globals.page = GAME_PAGE_WIN_GAME;
                game->globals.currentLevel = GAME_FIRST_LEVEL;

                Player_ResetStats(&game->globals.player);
            }
                
        } else {
            // Jika game mode adalah survival, cek jika waktu sudah habis
            if (game->globals.timeLeft == 0) {
                if (game->globals.currentLevel < GAME_LEVEL_COUNT) {
                    game->globals.page = GAME_PAGE_WIN_LEVEL;
                    game->globals.currentLevel++;
                } else {
                    game->globals.page = GAME_PAGE_WIN_GAME;
                    game->globals.currentLevel = GAME_FIRST_LEVEL;

                    Player_ResetStats(&game->globals.player);
                }
            }
        }
    }

    // Jika waktu habis dan game mode bukan survival, set nyawa player = 0
    if (game->globals.timeLeft == 0 && game->globals.mode != GAME_MODE_SURVIVAL)
        game->globals.player.health = 0;

    // Jika health player habis
    if (game->globals.player.health <= 0) {
        // Jika player masih memiliki sisa lives, tampilkan lose level page
        if (game->globals.player.lives > 0) {
            game->globals.player.health = PLAYER_STARTING_HEALTH;
            game->globals.player.lives--;
            game->globals.player.fireball = PLAYER_STARTING_FIREBALL;

            game->globals.page = GAME_PAGE_LOSE_LEVEL;
        } else { // Jika player tidak memiliki sisa lives, tampilkan lose game page
            game->globals.page = GAME_PAGE_LOSE_GAME;
            game->globals.currentLevel = GAME_FIRST_LEVEL;
         
            Player_ResetStats(&game->globals.player);
        }
    }

    // Pindahkah kursor mouse ke tengah layar
    SDL_WarpMouseInWindow(game->window, WINDOW_HW, WINDOW_HH);

    // Update timer game
    if (game->globals.lastTicks < SDL_GetTicks()) {
        game->globals.lastTicks = SDL_GetTicks() + 1000; // Update timer

        if (game->globals.timeLeft > 0)
            game->globals.timeLeft--;

        // Update time spent
        game->stats.timeSpent++;
        game->stats.totalTimeSpent++;
    }
}

// Fungsi untuk merender menu
static void _Game_RenderMenu(Game *game) {
    UI_DrawBackground(game);

    for (int i = 0; ; i++) {
        Button *button = MENU_BUTTONS[game->globals.page][i];

        if (button == NULL)
            break;
        
        Button_Render(game, button);
    }

    // Khusus untuk help menu
    if (game->globals.page == GAME_PAGE_HELP) {
        int textStartX = WINDOW_HW - 228;
        int textStartY = 80;

        const char **helpLinesArray = HELP_MENU_LINES[game->globals.helpPage];

        for (int i = 0; ; i++) {
            if (*helpLinesArray[i] == '\0') break;

            Font_Draw(game->renderer, helpLinesArray[i], textStartX, textStartY + FONT_CHAR_H * i, 0);
        }
        
        if (game->globals.helpPage < 2)
            Button_Render(game, &BUTTON_HELP_MENU_NEXT);
        
        if (game->globals.helpPage > 0)
            Button_Render(game, &BUTTON_HELP_MENU_PREVIOUS);
    }

    // Khusus untuk win level page untuk mode game selain survival
    if (game->globals.page == GAME_PAGE_WIN_LEVEL && game->globals.mode != GAME_MODE_SURVIVAL) {
        char winText[64];

        snprintf(winText, 64, "You got %d bonus points for the remaining %d seconds of time!", game->globals.timeLeft * 10, game->globals.timeLeft);
        Font_Draw(game->renderer, winText, WINDOW_HW, 192, 1);
    }

    // Khusus untuk win level dan lose level page
    if (game->globals.page >= GAME_PAGE_WIN_LEVEL) {
        char statsText[64];

        const char *statsMessage[7] = {
            "Enemies Killed",
            "Items Taken",
            "Score Got",
            "Fireballs Shot",
            "Damage Dealt",
            "Damage Received",
            "Time Spent",
        };

        int statsData[7] = {
            (game->globals.page < GAME_PAGE_WIN_GAME) ? game->stats.enemiesKilled : game->stats.totalEnemiesKilled,
            (game->globals.page < GAME_PAGE_WIN_GAME) ? game->stats.itemsTaken : game->stats.totalItemsTaken,
            (game->globals.page < GAME_PAGE_WIN_GAME) ? game->stats.scoreGot : game->stats.totalScoreGot,
            (game->globals.page < GAME_PAGE_WIN_GAME) ? game->stats.fireballsShot : game->stats.totalFireballsShot,
            (game->globals.page < GAME_PAGE_WIN_GAME) ? game->stats.damageDealt : game->stats.totalDamageDealt,
            (game->globals.page < GAME_PAGE_WIN_GAME) ? game->stats.damageReceived : game->stats.totalDamageReceived,
            (game->globals.page < GAME_PAGE_WIN_GAME) ? game->stats.timeSpent : game->stats.totalTimeSpent,
        };

        for (int i = 0; i < 7; i++) {
            if (game->globals.page >= GAME_PAGE_WIN_GAME)
                if (i < 6) // Untuk data selain time spent
                    snprintf(statsText, 64, "Total %s : %d", statsMessage[i], statsData[i]);
                else
                    snprintf(statsText, 64, "Total %s : %2d minute(s) %2d second(s)", statsMessage[i], statsData[i] / 60, statsData[i] % 60);
            else
                if (i < 6) // Untuk data selain time spent
                    snprintf(statsText, 64, "%s : %d", statsMessage[i], statsData[i]);
                else
                    snprintf(statsText, 64, "%s : %2d minute(s) %2d second(s)", statsMessage[i], statsData[i] / 60, statsData[i] % 60);

            Font_Draw(game->renderer, statsText, WINDOW_HW, 64 + 16 * i, 1);
        }
    }
}

// Fungsi untuk rendering game
static void _Game_RenderMain(Game *game) {
    // Gambar game world
    SDL_LockTexture(game->texture, NULL, (void**) &game->texturePixels, &game->texturePitch);

    Raycaster_Render(game->texturePixels, &game->globals.player);
    Item_Render(game);
    Fireball_Render(&game->globals.player);
    Enemy_Render(game);
    Decoration_Render(game);
    Sprite_Render(game, &game->globals.player);

    SDL_UnlockTexture(game->texture);

    SDL_RenderCopy(game->renderer, game->texture, NULL, NULL);

    // Tampilkan minimap
    if (game->globals.minimap)
        Minimap_Render(game);

    char uiText[64];

    // Tampilkan health player
    snprintf(uiText, 64, "Health : %d", game->globals.player.health);
    Font_Draw(game->renderer, uiText, 16, 16, 0);

    // Tampilkan lives player
    snprintf(uiText, 64, "Lives : %d", game->globals.player.lives);
    Font_Draw(game->renderer, uiText, 16, 32, 0);

    // Tampilkan score player
    snprintf(uiText, 64, "Score : %d", game->globals.player.score);
    Font_Draw(game->renderer, uiText, 16, 48, 0);

    // Tampilkan jumlah fireball player
    snprintf(uiText, 64, "Fireballs : %d", game->globals.player.fireball);
    Font_Draw(game->renderer, uiText, 16, 64, 0);

    // Tampilkan timer game
    unsigned timeLeft = game->globals.timeLeft;
    snprintf(uiText, 64, "Time left : %d:%02d", timeLeft / 60, timeLeft % 60);
    Font_Draw(game->renderer, uiText, 16, 96, 0);

    // Tampilkan speed boost / slowness timer
    if (game->globals.player.speedTimer) {
        if (game->globals.player.speed > 1.0)
            snprintf(uiText, 64, "Speed boost : %d", game->globals.player.speedTimer / 60);
        else if(game->globals.player.speed < 1.0)
            snprintf(uiText, 64, "Slowness : %d", game->globals.player.speedTimer / 60);

        Font_Draw(game->renderer, uiText, 16, 112, 0);
    }

    // Tampilkan x - ray / blindness timer
    if (game->globals.player.visionTimer) {
        if (game->globals.player.xray)
            snprintf(uiText, 64, "X - Ray : %d", game->globals.player.visionTimer / 60);
        else
            snprintf(uiText, 64, "Blindness : %d", game->globals.player.visionTimer / 60);

        Font_Draw(game->renderer, uiText, 16, 128, 0);
    }

    // Tampilkan level
    snprintf(uiText, 64, "Level %d", game->globals.currentLevel);
    Font_Draw(game->renderer, uiText, 16, 144, 0);

    // Tampilkan current game mode
    switch (game->globals.mode) {
        case GAME_MODE_SURVIVAL:
            snprintf(uiText, 64, "Game mode : Survival");
            break;
        
        case GAME_MODE_PACIFIST:
            snprintf(uiText, 64, "Game mode : Pacifist");
            break;
        
        default:
            snprintf(uiText, 64, "Game mode : Escape");
            break;
    }

    Font_Draw(game->renderer, uiText, 16, 160, 0);
}

void Game_InitMenu(void) {
    Button_Init(&BUTTON_MAIN_MENU_START,
        WINDOW_HW, WINDOW_HH - GAME_MENU_BUTTON_H - 16,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Start Game", 1,
        _Game_ButtonMainMenuStart
    );

    Button_Init(&BUTTON_MAIN_MENU_HELP,
        WINDOW_HW, WINDOW_HH,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Help", 1,
        _Game_ButtonMainMenuHelp
    );

    Button_Init(&BUTTON_MAIN_MENU_EXIT,
        WINDOW_HW, WINDOW_HH + GAME_MENU_BUTTON_H + 16,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Exit", 1,
        _Game_ButtonMainMenuExit
    );

    Button_Init(&BUTTON_DIFFICULTY_EASY,
        WINDOW_HW, WINDOW_HH - GAME_MENU_BUTTON_H - 16,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Easy", 1,
        _Game_ButtonDifficultyEasy
    );

    Button_Init(&BUTTON_DIFFICULTY_MEDIUM,
        WINDOW_HW, WINDOW_HH,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Medium", 1,
        _Game_ButtonDifficultyMedium
    );

    Button_Init(&BUTTON_DIFFICULTY_HARD,
        WINDOW_HW, WINDOW_HH + GAME_MENU_BUTTON_H + 16,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Hard", 1,
        _Game_ButtonDifficultyHard
    );

    Button_Init(&BUTTON_HELP_MENU_NEXT,
        WINDOW_HW + GAME_MENU_BUTTON_W + 16, 400,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Next", 1,
        _Game_ButtonHelpMenuNext
    );

    Button_Init(&BUTTON_HELP_MENU_PREVIOUS,
        WINDOW_HW - GAME_MENU_BUTTON_W - 16, 400,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Previous", 1,
        _Game_ButtonHelpMenuPrevious
    );

    Button_Init(&BUTTON_MAIN_MENU,
        WINDOW_HW, 400,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Main Menu", 1,
        _Game_ButtonMainMenu
    );

    Button_Init(&BUTTON_WIN_LEVEL_CONTINUE,
        WINDOW_HW, WINDOW_HH,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Next Level", 1,
        _Game_ButtonNextLevelRestart
    );

    Button_Init(&BUTTON_LOSE_LEVEL_RESTART,
        WINDOW_HW, WINDOW_HH,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Restart Level", 1,
        _Game_ButtonNextLevelRestart
    );


    Button_Init(&BUTTON_RESUME,
        WINDOW_HW, WINDOW_HH,
        GAME_MENU_BUTTON_W, GAME_MENU_BUTTON_H,
        "Resume Game", 1,
        _Game_ButtonResume
    );


    Button_Init(&TITLE_MAIN_MENU,
        WINDOW_HW, GAME_MENU_TITLE_Y,
        GAME_MENU_TITLE_W, GAME_MENU_TITLE_H,
        "Maze Escape", 1,
        NULL
    );

    Button_Init(&TITLE_DIFFICULTY,
        WINDOW_HW, GAME_MENU_TITLE_Y,
        GAME_MENU_TITLE_W, GAME_MENU_TITLE_H,
        "Choose Difficulty", 1,
        NULL
    );

    Button_Init(&TITLE_HELP,
        WINDOW_HW, GAME_MENU_TITLE_Y,
        GAME_MENU_TITLE_W, GAME_MENU_TITLE_H,
        "Game Help", 1,
        NULL
    );

    Button_Init(&TITLE_PAUSE,
        WINDOW_HW, GAME_MENU_TITLE_Y,
        GAME_MENU_TITLE_W, GAME_MENU_TITLE_H,
        "Pause Menu", 1,
        NULL
    );

    Button_Init(&TITLE_WIN_LEVEL,
        WINDOW_HW, GAME_MENU_TITLE_Y,
        GAME_MENU_TITLE_W, GAME_MENU_TITLE_H,
        "You won this level!", 1,
        NULL
    );

    Button_Init(&TITLE_WIN_GAME,
        WINDOW_HW, GAME_MENU_TITLE_Y,
        GAME_MENU_TITLE_W, GAME_MENU_TITLE_H,
        "You won the game!", 1,
        NULL
    );

    Button_Init(&TITLE_LOSE_LEVEL,
        WINDOW_HW, GAME_MENU_TITLE_Y,
        GAME_MENU_TITLE_W, GAME_MENU_TITLE_H,
        "You lost this level!", 1,
        NULL
    );

    Button_Init(&TITLE_LOSE_GAME,
        WINDOW_HW, GAME_MENU_TITLE_Y,
        GAME_MENU_TITLE_W, GAME_MENU_TITLE_H,
        "You lost the game!", 1,
        NULL
    );
}

void Game_Start(Game *game, GameDifficulty difficulty) {
    // Set difficulty dari game
    difficulty = SDL_clamp(difficulty, GAME_DIFFICULTY_EASY, GAME_DIFFICULTY_HARD);

    // Simpan tingkat kesulitan yang digunakan
    game->globals.difficulty = difficulty;

    switch (difficulty) {
        case GAME_DIFFICULTY_MEDIUM:
            game->settings = &GAME_SETTINGS_MEDIUM;
            break;
        
        case GAME_DIFFICULTY_HARD:
            game->settings = &GAME_SETTINGS_HARD;
            break;
        
        default:
            game->settings = &GAME_SETTINGS_EASY;
            break;
    }
    
    // Randomize game mode
    game->globals.mode = (game->globals.mode + 1 + (rand() % 2)) % 3;

    // Jika game mode pacifist, pemain tidak bisa menembak fireball
    if (game->globals.mode == GAME_MODE_PACIFIST)
        game->globals.player.isPacifist = 1;
    else
        game->globals.player.isPacifist = 0;

    // Mulai timer game
    if (game->globals.mode != GAME_MODE_SURVIVAL)
        game->globals.timeLeft = game->settings->timeLimit;
    else
        game->globals.timeLeft = game->settings->survivalTimeLimit;

    game->globals.lastTicks = SDL_GetTicks() + 1000;

    // Inisialisasi map
    if (!Map_Init(game)) {
        return;
    }

    // Inisialisasi posisi pemain
    Player_SetPosition(&game->globals.player);

    // Reset semua level stats
    game->stats.enemiesKilled = 0;
    game->stats.itemsTaken = 0;
    game->stats.scoreGot = 0;
    game->stats.fireballsShot = 0;
    game->stats.damageDealt = 0;
    game->stats.damageReceived = 0;
    game->stats.timeSpent = 0;

    // Reset game stats jika memulai dari awal
    if (game->globals.currentLevel == 1) {    
        Player_ResetStats(&game->globals.player);
        
        game->stats.totalEnemiesKilled = 0;
        game->stats.totalItemsTaken = 0;
        game->stats.totalScoreGot = 0;
        game->stats.totalFireballsShot = 0;
        game->stats.totalDamageDealt = 0;
        game->stats.totalDamageReceived = 0;
        game->stats.totalTimeSpent = 0;
    }

    // Set game page
    game->globals.page = GAME_PAGE_MAIN;
}

void Game_Update(Game *game) {
    if (game->globals.page == GAME_PAGE_MAIN) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
        _Game_UpdateMain(game);
    } else {
        SDL_SetRelativeMouseMode(SDL_FALSE);
        _Game_UpdateMenu(game);
    }
}

void Game_Render(Game *game) {
    if (game->globals.page == GAME_PAGE_MAIN) {
        _Game_RenderMain(game);
    } else {
        _Game_RenderMenu(game);
    }
}

int Game_AddPlayerScore(Game *game, int score) {
    int scoreMod = game->globals.player.score % PLAYER_LIFE_BONUS_SCORE;
    
    game->globals.player.score += score;

    return (scoreMod + score) / PLAYER_LIFE_BONUS_SCORE;
}