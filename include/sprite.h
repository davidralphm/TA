#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "player.h"
#include "game.h"

#define SPRITE_COUNT_MAX 256

typedef struct sprite_t {
    // Posisi sprite dalam world
    double x, y;

    // Jarak dari sprite, digunakan untuk melakukan sorting
    double dist;

    // Koordinat proyeksi sprite
    int x1, x2;
    int y1, y2;

    // ID texture dari sprite
    unsigned char textureId;

    // Koordinat texture dan texture step dari sprite
    double texX, texY;
    double texXStep, texYStep;
} Sprite;

// Fungsi untuk menambahkan sprite
void Sprite_Add(Player *player, double spriteX, double spriteY, unsigned char textureId, double scale);

// Fungsi untuk menggambar semua sprite dalam game
void Sprite_Render(Game *game, Player *player);

#endif