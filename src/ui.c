#include "ui.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

// Texture button
static SDL_Texture *BUTTON_TEXTURE;

// Texture background
static SDL_Texture *BACKGROUND_TEXTURE;

int UI_Init(Game *game) {
    SDL_Surface *buttonSurface = IMG_Load("./res/button.png");
    SDL_Surface *bgSurface = IMG_Load("./res/background.png");

    if (buttonSurface == NULL || bgSurface == NULL) {
        printf("Gagal loading UI textures: %s\n", IMG_GetError());

        return 0;
    }

    BUTTON_TEXTURE = SDL_CreateTextureFromSurface(game->renderer, buttonSurface);
    BACKGROUND_TEXTURE = SDL_CreateTextureFromSurface(game->renderer, bgSurface);

    SDL_FreeSurface(buttonSurface);
    SDL_FreeSurface(bgSurface);

    if (BUTTON_TEXTURE == NULL || BACKGROUND_TEXTURE == NULL) {
        printf("Gagal membuat UI textures: %s\n", SDL_GetError());

        return 0;
    }

    return 1;
}

void UI_Quit(void) {
    if (BUTTON_TEXTURE) SDL_DestroyTexture(BUTTON_TEXTURE);
    if (BACKGROUND_TEXTURE) SDL_DestroyTexture(BACKGROUND_TEXTURE);
}

void UI_DrawButton(Game *game, SDL_Rect *srcRect, SDL_Rect *dstRect) {
    SDL_RenderCopy(game->renderer, BUTTON_TEXTURE, srcRect, dstRect);
}

void UI_DrawBackground(Game *game) {
    SDL_RenderCopy(game->renderer, BACKGROUND_TEXTURE, NULL, NULL);
}
