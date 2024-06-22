#include "ui.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

// Texture button
static SDL_Texture *BUTTON_TEXTURE;

// Texture background
static SDL_Texture *BACKGROUND_TEXTURE;

// Texture untuk animasi win dan lose menu
static SDL_Texture *MENU_ANIMS_TEXTURE;

int UI_Init(Game *game) {
    SDL_Surface *buttonSurface = IMG_Load("./res/button.png");
    SDL_Surface *bgSurface = IMG_Load("./res/background.png");
    SDL_Surface *menuAnimSurface = IMG_Load("./res/menuanims.png");

    if (buttonSurface == NULL || bgSurface == NULL || menuAnimSurface == NULL) {
        printf("Gagal loading UI textures: %s\n", IMG_GetError());

        if (buttonSurface != NULL) SDL_FreeSurface(buttonSurface);
        if (bgSurface != NULL) SDL_FreeSurface(bgSurface);
        if (menuAnimSurface != NULL) SDL_FreeSurface(menuAnimSurface);

        return 0;
    }

    SDL_SetColorKey(menuAnimSurface, SDL_TRUE, 0x00FF00FF);

    BUTTON_TEXTURE = SDL_CreateTextureFromSurface(game->renderer, buttonSurface);
    BACKGROUND_TEXTURE = SDL_CreateTextureFromSurface(game->renderer, bgSurface);
    MENU_ANIMS_TEXTURE = SDL_CreateTextureFromSurface(game->renderer, menuAnimSurface);

    SDL_FreeSurface(buttonSurface);
    SDL_FreeSurface(bgSurface);
    SDL_FreeSurface(menuAnimSurface);

    if (BUTTON_TEXTURE == NULL || BACKGROUND_TEXTURE == NULL || MENU_ANIMS_TEXTURE == NULL) {
        printf("Gagal membuat UI textures: %s\n", SDL_GetError());

        return 0;
    }

    return 1;
}

void UI_Quit(void) {
    if (BUTTON_TEXTURE) SDL_DestroyTexture(BUTTON_TEXTURE);
    if (BACKGROUND_TEXTURE) SDL_DestroyTexture(BACKGROUND_TEXTURE);
    if (MENU_ANIMS_TEXTURE) SDL_DestroyTexture(MENU_ANIMS_TEXTURE);
}

void UI_DrawButton(Game *game, SDL_Rect *srcRect, SDL_Rect *dstRect) {
    SDL_RenderCopy(game->renderer, BUTTON_TEXTURE, srcRect, dstRect);
}

void UI_DrawBackground(Game *game) {
    switch (game->globals.page) {
        case GAME_PAGE_LOSE_GAME:
        case GAME_PAGE_LOSE_LEVEL:
            SDL_SetTextureColorMod(BACKGROUND_TEXTURE, 255, 128, 128);
            break;
        
        case GAME_PAGE_WIN_GAME:
        case GAME_PAGE_WIN_LEVEL:
            SDL_SetTextureColorMod(BACKGROUND_TEXTURE, 128, 255, 128);
            break;

        default:
            SDL_SetTextureColorMod(BACKGROUND_TEXTURE, 255, 255, 255);
    }

    SDL_RenderCopy(game->renderer, BACKGROUND_TEXTURE, NULL, NULL);
}

void UI_DrawMenuAnim(Game *game, unsigned char animId, int x, int y) {
    animId &= 1;

    SDL_Rect srcRect = {
        (animId << 8) + (((SDL_GetTicks() >> 8) & 3) << 6), 0,
        64, 64
    };

    SDL_Rect dstRect = { x, y, 64, 64 };

    SDL_RenderCopy(game->renderer, MENU_ANIMS_TEXTURE, &srcRect, &dstRect);
}
