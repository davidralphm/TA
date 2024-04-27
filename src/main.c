#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <math.h>

#include "player.h"
#include "raycaster.h"
#include "map.h"
#include "textures.h"
#include "sprite.h"
#include "item.h"
#include "game.h"
#include "font.h"
#include "game.h"
#include "enemy.h"
#include "enemyspawner.h"
#include "sounds.h"
#include "ui.h"
#include "teleportexit.h"

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

// Maximum FPS
#define MAX_FPS 60.0

// Variabel game
static Game GAME;

// Variable delta timing agar frame rate game update konsisten
static const double UPDATE_RATE = 1.0 / MAX_FPS;
static double accumulator = 0.0, timeDiff = 0.0;
static unsigned currTime, lastTime;

// Fungsi untuk menginisialisasi semua resources
// yang digunakan pada game
static int init(void) {
    // Inisialisasi SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Gagal menginisialisasi SDL2: %s\n", SDL_GetError());

        return 0;
    }

    // Inisialisasi SDL Image
    if (!IMG_Init(IMG_INIT_PNG)) {
        printf("Gagal menginisialisasi SDL Image: %s\n", IMG_GetError());

        return 0;
    }

    // Inisialisasi SDL Mixer
    if (!Mix_Init(MIX_INIT_MP3)) {
        printf("Gagal menginisialisasi SDL Mixer: %s\n", Mix_GetError());

        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 2048)) {
        printf("Gagal open audio SDL Mixer: %s\n", Mix_GetError());

        return 0;
    }

    // Load semua sound effect
    if (!Sounds_Load()) {
        return 0;
    }

    // Buat window dari game
    GAME.window = SDL_CreateWindow(
        "Maze Escape",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H,
        0
    );

    if (GAME.window == NULL) {
        printf("Gagal membuat Window: %s\n", SDL_GetError());

        return 0;
    }

    // Buat renderer
    GAME.renderer = SDL_CreateRenderer(GAME.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (GAME.renderer == NULL) {
        printf("Gagal membuat Renderer: %s\n", SDL_GetError());

        return 0;
    }

    // Buat texture
    GAME.texture = SDL_CreateTexture(
        GAME.renderer,
        SDL_GetWindowPixelFormat(GAME.window),
        SDL_TEXTUREACCESS_STREAMING,
        VIEWPORT_W, VIEWPORT_H
    );

    if (GAME.texture == NULL) {
        printf("Gagal membuat Texture: %s\n", SDL_GetError());

        return 0;
    }

    // Inisialisasi font
    if (!Font_Init(GAME.renderer))
        return 0;

    // Inisialisasi game textures
    if (!Textures_Init()) {
        return 0;
    }

    // Inisialisasi UI textures
    if (!UI_Init(&GAME)) {
        return 0;
    }

    // Inisialisasi game menus
    Game_InitMenu();

    // Reset level
    GAME.globals.currentLevel = 1;

    // Inisialisasi player
    Player_Init(&GAME.globals.player, 0.0, 0.0, 0.0, M_PI / 2.5);

    printf("Inisialisasi berhasil!\n");

    // Untuk delta timing
    lastTime = SDL_GetTicks();

    return 1;
}

// Fungsi main loop dari game
static void loop(void) {
    SDL_Event event;

#ifndef __EMSCRIPTEN__
    int running = 1;
    while (running) {
#endif
        // Reset mouse click
        GAME.globals.player.mouseClick = 0;
        
        // Process semua events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
#ifndef __EMSCRIPTEN__
                case SDL_QUIT:
                    running = 0;
                    break;
#endif
                
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        // case SDLK_ESCAPE:
                        case SDLK_BACKQUOTE:
                            if (GAME.globals.page == GAME_PAGE_MAIN)
                                GAME.globals.page = GAME_PAGE_PAUSE;

                            break;

                        // Toggle minimap
                        case SDLK_m:
                            GAME.globals.minimap = !GAME.globals.minimap;
                            Sounds_Play(SOUND_EFFECT_BUTTON);

                            break;

                        default:
                            break;
                    }

                    break;
                
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                        GAME.globals.player.mouseClick = 1;
                    else
                        GAME.globals.player.mouseClick = 0;
                    
                    SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);

                    break;
                
                case SDL_MOUSEBUTTONUP:
                    GAME.globals.player.mouseClick = 0;
                    break;

                default:
                    break;
            }
        }

        // Perhitungan delta timing
        currTime = SDL_GetTicks();
        timeDiff = (double) (currTime - lastTime) / 1000.0;
        accumulator += timeDiff;
        lastTime = currTime;

        // Update game
        while (accumulator >= UPDATE_RATE) {
            Game_Update(&GAME);

            accumulator -= UPDATE_RATE;
        }

        // Render game
        Game_Render(&GAME);

        SDL_RenderPresent(GAME.renderer);
#ifndef __EMSCRIPTEN__
    }
#endif
}

// Fungsi untuk menghapus semua resources yang sudah diload
static void quit(void) {
    if (GAME.texture != NULL) SDL_DestroyTexture(GAME.texture);
    if (GAME.renderer != NULL) SDL_DestroyRenderer(GAME.renderer);
    if (GAME.window != NULL) SDL_DestroyWindow(GAME.window);

    UI_Quit();
    Textures_Quit();

    Font_Quit();

    Mix_CloseAudio();
    Sounds_Quit();

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    printf("Berhasil quit game!\n");
}

int main(int argc, char **argv) {
    if (!init()) {
        printf("Gagal menginisialisasi game!\n");

        quit();

        return 1;
    }

#ifndef __EMSCRIPTEN__
    loop();
#else
    emscripten_set_main_loop(loop, 0, -1);
#endif

    quit();
    return 0;
}