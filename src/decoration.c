#include "decoration.h"
#include "sprite.h"
#include "map.h"

// Jumlah maksimum objek dekorasi dalam game
#define DECORATION_COUNT_MAX 64

// Array objek dekorasi dalam game
static Decoration DECORATION[DECORATION_COUNT_MAX];

// Jumlah objek dekorasi dalam game
static unsigned DECORATION_COUNT = 0;

Decoration* Decoration_New(double x, double y, double scale, DecorationName object, unsigned char obstacle) {
    if (DECORATION_COUNT >= DECORATION_COUNT_MAX) return NULL;

    Decoration *decoration = &DECORATION[DECORATION_COUNT++];

    decoration->x = x;
    decoration->y = y;
    decoration->scale = scale;

    // Set texture dari objek dekorasi
    object = SDL_clamp(object, DECORATION_BARREL_1, DECORATION_TORCH);
    decoration->textureId = TEXTURE_ID_BARREL_1 + object;

    // Jika objek bersifat obstacle, maka cell yang ditempati
    // oleh objek tersebut tidak dapat dilewati pemain atau musuh
    Map_Get((unsigned) x, (unsigned) y)->isObstacle = obstacle != 0;

    return decoration;
}

void Decoration_Render(Game *game) {
    for (int i = 0; i < DECORATION_COUNT; i++) {
        Decoration *decoration = &DECORATION[i];

        // Animasi torch
        if (decoration->textureId >= TEXTURE_ID_TORCH_FRAME_1) {
            decoration->textureId = TEXTURE_ID_TORCH_FRAME_1 + ((SDL_GetTicks() >> 7) % 4);
        }

        Sprite_Add(&game->globals.player, decoration->x, decoration->y, decoration->textureId, decoration->scale);
    }
}

void Decoration_DeleteAll(void) {
    DECORATION_COUNT = 0;
}
