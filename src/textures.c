#include "textures.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

static SDL_Surface *TEXTURES;

int Textures_Init(void) {
    SDL_Surface *tex = IMG_Load("./res/texturesheet.png");

    if (tex == NULL) {
        printf("Gagal loading textures: %s\n", IMG_GetError());

        return 0;
    }

    TEXTURES = SDL_ConvertSurfaceFormat(tex, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(tex);
    
    if (TEXTURES == NULL) {
        printf("Gagal loading textures: %s\n", IMG_GetError());

        return 0;
    }

    SDL_LockSurface(TEXTURES);

    return 1;
}

unsigned Textures_Get(unsigned x, unsigned y) {
    x = SDL_clamp(x, 0, TEXTURES->w - 1);
    y = SDL_clamp(y, 0, TEXTURES->h - 1);

    return *((unsigned*) TEXTURES->pixels + y * TEXTURES->w + x);
}

void Textures_Quit(void) {
    if (TEXTURES != NULL) {
        SDL_UnlockSurface(TEXTURES);
        SDL_FreeSurface(TEXTURES);
    }
}
