#include "font.h"

#include <stdio.h>
#include <string.h>
#include <SDL2/SDL_image.h>

// Texture untuk font
static SDL_Texture *FONT_TEXTURE;

int Font_Init(SDL_Renderer *renderer) {
    if (renderer == NULL) return 0;

    SDL_Surface *fontSurface = IMG_Load("./res/font.png");

    if (fontSurface == NULL) {
        printf("Gagal loading bitmap font: %s\n", IMG_GetError());

        return 0;
    }

    SDL_SetColorKey(fontSurface, SDL_TRUE, 0);
    FONT_TEXTURE = SDL_CreateTextureFromSurface(renderer, fontSurface);
    SDL_FreeSurface(fontSurface);

    if (FONT_TEXTURE == NULL) {
        printf("Gagal membuat texture bitmap font: %s\n", SDL_GetError());

        return 0;
    }

    return 1;
}

void Font_Draw(SDL_Renderer *renderer, const char *text, int x, int y, char centered) {
    SDL_Rect fontRect = { 0, 0, FONT_CHAR_W, FONT_CHAR_H };
    SDL_Rect destRect = { x, y, FONT_CHAR_W, FONT_CHAR_H };

    if (centered) {
        destRect.x -= strlen(text) * FONT_CHAR_W / 2;
        destRect.y -= (FONT_CHAR_H / 2);
    }

    while (*text) {
        int offsetX = SDL_clamp(*text - ' ', 0, 95);
        fontRect.x = offsetX * FONT_CHAR_W;

        SDL_RenderCopy(renderer, FONT_TEXTURE, &fontRect, &destRect);

        destRect.x += FONT_CHAR_W;
        text++;
    }
}

void Font_Quit(void) {
    if (FONT_TEXTURE)
        SDL_DestroyTexture(FONT_TEXTURE);
}
