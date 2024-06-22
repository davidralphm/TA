#ifndef __UI_H__
#define __UI_H__

#include "game.h"

// Fungsi untuk loading UI textures
int UI_Init(Game *game);

// Fungsi untuk menghapus texture UI
void UI_Quit(void);

// Fungsi untuk menggambar texture button
void UI_DrawButton(Game *game, SDL_Rect *srcRect, SDL_Rect *dstRect);

// Fungsi untuk menggambar background menu
void UI_DrawBackground(Game *game);

// Fungsi untuk menggambar animasi pada win dan lose menu
void UI_DrawMenuAnim(Game *game, unsigned char animId, int x, int y);

#endif