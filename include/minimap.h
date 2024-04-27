#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include "game.h"

// Tipe objek pada minimap
typedef struct minimap_object_t {
    double x, y;
    unsigned char r, g, b;
} MinimapObject;

// Fungsi untuk menambahkan objek yang dirender pada minimap
void Minimap_AddObject(Game *game, double x, double y, unsigned char r, unsigned char g, unsigned char b);

// Fungsi untuk merender minimap
void Minimap_Render(Game *game);

#endif