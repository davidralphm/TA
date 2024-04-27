#ifndef __DECORATIONS_H__
#define __DECORATIONS_H__

#include "game.h"
#include "textures.h"

// Enum untuk objek - objek dekorasi
typedef enum decoration_object_e {
    DECORATION_BARREL_1,
    DECORATION_BARREL_2,
    DECORATION_BOXES,
    DECORATION_CHAIR_1,
    DECORATION_CHAIR_2,
    DECORATION_LAMP,
    DECORATION_BONES,
    DECORATION_TABLE,
    DECORATION_FLAG,
    DECORATION_TORCH,
} DecorationName;

// Tipe objek dekorasi
typedef struct decoration_t {
   double x, y;
   double scale;

   TextureID textureId;
} Decoration;

// Fungsi untuk membuat objek dekorasi baru
Decoration* Decoration_New(double x, double y, double scale, DecorationName object, unsigned char obstacle);

// Fungsi untuk menggambar semua objek dekorasi dalam game
void Decoration_Render(Game *game);

// Fungsi untuk menghapus semua objek dekorasi dalam game
void Decoration_DeleteAll(void);

#endif