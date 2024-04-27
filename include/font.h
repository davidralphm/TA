#ifndef __FONT_H__
#define __FONT_H__

#include <SDL2/SDL.h>

// Width dari tiap character
#define FONT_CHAR_W 8

// Height dari tiap character
#define FONT_CHAR_H 16

// Fungsi untuk loading bitmap font
int Font_Init(SDL_Renderer *renderer);

// Fungsi untuk menggambar teks
void Font_Draw(SDL_Renderer *renderer, const char *text, int x, int y, char centered);

// Fungsi untuk quit font
void Font_Quit(void);

#endif