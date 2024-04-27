#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "game.h"

#include <SDL2/SDL.h>

// Panjang maksimum teks untuk button
#define BUTTON_TEXT_LEN_MAX 64

// Tipe button yang digunakan dalam menu - menu game
typedef struct button_t {
    SDL_Rect buttonRect;
    char buttonText[BUTTON_TEXT_LEN_MAX];

    void (*callback)(Game *game);
} Button;

// Fungsi untuk menginisialisasi button
void Button_Init(Button *button, int x, int y, int w, int h, const char *buttonText, char centered, void (*callback)(Game *game));

// Fungsi untuk mengupdate sebuah button
void Button_Update(Game *game, Button *button, int mouseX, int mouseY, int mouseClick);

// Fungsi untuk merender sebuah button
void Button_Render(Game *game, Button *button);

#endif