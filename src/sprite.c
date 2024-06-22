#include "sprite.h"

#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

#include "raycaster.h"
#include "textures.h"
#include "minimap.h"

// Array sprite yang ada pada game
static Sprite SPRITE[SPRITE_COUNT_MAX];

// Array yang berisi pointer sprite yang sudah diurutkan
// berdasarkan jaraknya terhadap pemain
static Sprite *SPRITE_ORDER[SPRITE_COUNT_MAX];

// Jumlah sprite yang ada di dalam game
static unsigned SPRITE_COUNT = 0;

// Fungsi untuk membandingkan jarak sprite untuk sorting
static int Sprite_Compare(const void *a, const void *b) {
    Sprite *spriteA = *((Sprite**) a);
    Sprite *spriteB = *((Sprite**) b);

    return (int) (spriteA->dist * 1000.0 - spriteB->dist * 1000.0);
}

// Fungsi untuk sorting sprite
static void _Sprite_Sort(void) {
    qsort((void*) SPRITE_ORDER, SPRITE_COUNT, sizeof(Sprite*), Sprite_Compare);
}

// Fungsi untuk merender satu sprite
static void _Sprite_Render(Game *game, Sprite *sprite, Player *player) {
    unsigned texXOffset = sprite->textureId * TEXTURE_SIZE;
    unsigned char visible = 0;

    for (int x = sprite->x1; x <= sprite->x2; x++) {
        // Hitung texture x coordinate dari sprite
        unsigned char texX = (int) (sprite->texX * (double) TEXTURE_SIZE) & TEXTURE_SIZE_MASK;
        sprite->texX += sprite->texXStep;
        
        double texYCoord = sprite->texY;
        
        // Jika slice dari sprite berada di belakang tembok, lanjut ke slice berikutnya
        if (Raycaster_GetWallDist(x) <= sprite->dist && player->xray == 0) continue;

        // Jika ada paling tidak satu slice dari sprite yang tidak tertutupi slice tembok,
        // maka sprite dianggap visible
        visible = 1;

        for (int y = sprite->y1; y <= sprite->y2; y++) {
            // Hitung texture y coordinate dari sprite
            unsigned char texY = (int) (texYCoord * TEXTURE_SIZE) & TEXTURE_SIZE_MASK;
            texYCoord += sprite->texYStep;

            unsigned pixel = Textures_Get(texX + texXOffset, texY) & 0x00FFFFFF;

            if (pixel != 0x00FF00FF) {
                if (Raycaster_GetStencil(x, y) == 0) {
                    pixel = Raycaster_MultiplyPixel(pixel, SDL_clamp(1.0 - sprite->dist / player->fogDist, 0.0, 1.0));
                    
                    Raycaster_SetPixel(game->texturePixels, x, y, pixel);
                    Raycaster_SetStencil(x, y, 1);
                }
            }
        }
    }

    if (visible) {
        // Tentukan warna untuk minimap object berdasarkan jenis sprite
        unsigned char r, g, b;

        switch (sprite->textureId) {
            case TEXTURE_ID_TREASURE:
            case TEXTURE_ID_HEALTH:
            case TEXTURE_ID_LIFE:
            case TEXTURE_ID_FIREBALL:
            case TEXTURE_ID_TIME_BONUS:
            case TEXTURE_ID_SPEED_BOOST:
            case TEXTURE_ID_XRAY:
                r = 255;
                g = 255;
                b = 0;
                
                break;
            
            case TEXTURE_ID_TRAP:
                r = 128;
                g = 128;
                b = 128;

                break;
            
            case TEXTURE_ID_ENEMY_FRAME_1:
            case TEXTURE_ID_ENEMY_FRAME_2:
            case TEXTURE_ID_ENEMY_FRAME_3:
            case TEXTURE_ID_ENEMY_FRAME_4:
            case TEXTURE_ID_ENEMY_FRAME_5:
            case TEXTURE_ID_ENEMY_FRAME_6:
                r = 255;
                g = 0;
                b = 0;

                break;

            default:
                r = 0;
                g = 255;
                b = 255;

                break;
        };

        Minimap_AddObject(game, sprite->x, sprite->y, r, g, b);
    }
}

void Sprite_Add(Player *player, double spriteX, double spriteY, unsigned char textureId, double scale) {
    if (SPRITE_COUNT >= SPRITE_COUNT_MAX) return;

    Sprite *sprite = &SPRITE[SPRITE_COUNT];

    // Transformasikan posisi sprite terhadap pemain
    double relativeX = spriteX - player->x;
    double relativeY = spriteY - player->y;

    double transformedX = relativeX * cos(player->angle) - relativeY * sin(player->angle);
    double transformedY = relativeX * sin(player->angle) + relativeY * cos(player->angle);

    // Cek jarak sprite terhadap pemain
    if (transformedX < 0.001) return; // Sprite terlalu dekat dengan pemain atau berada di belakang pemain

    // Simpan jarak sprite
    sprite->dist = transformedX;

    // Hitung koordinat x dari proyeksi tengah sprite pada layar
    int midX = (transformedY * player->camPlaneDist) / sprite->dist + VIEWPORT_HW;

    // Hitung ukuran proyeksi sprite pada layar
    int projW = player->camPlaneDist / sprite->dist * scale;

    // Hitung koordinat proyeksi bagian kiri dan kanan sprite
    sprite->x1 = midX - (projW >> 1);
    sprite->x2 = sprite->x1 + projW;

    if (sprite->x2 < 0 || sprite->x1 >= VIEWPORT_W || sprite->x1 >= sprite->x2) return;

    // Hitung koordinat proyeksi y sprite pada layar
    int projH = scale * player->camPlaneDist / sprite->dist;

    sprite->y2 = 0.5 * player->camPlaneDist / sprite->dist + VIEWPORT_HH;
    sprite->y1 = sprite->y2 - projH;

    if (sprite->y2 < 0 || sprite->y1 >= VIEWPORT_H || sprite->y1 >= sprite->y2) return;

    // Hitung texture step x dan y dari sprite
    sprite->texXStep = 1.0 / (double) (projW);
    sprite->texYStep = 1.0 / (double) (projH);

    // Hitung texture coordinate x dan y dari sprite
    sprite->texX = (sprite->x1 >= 0) ? 0 : -sprite->x1 * sprite->texXStep;
    sprite->texY = (sprite->y1 >= 0) ? 0 : -sprite->y1 * sprite->texYStep;

    // Clamp koordinat proyeksi sprite
    sprite->x1 = SDL_clamp(sprite->x1, 0, VIEWPORT_W - 1);
    sprite->x2 = SDL_clamp(sprite->x2, 0, VIEWPORT_W - 1);

    sprite->y1 = SDL_clamp(sprite->y1, 0, VIEWPORT_H - 1);
    sprite->y2 = SDL_clamp(sprite->y2, 0, VIEWPORT_H - 1);

    // Simpan texture Id
    sprite->textureId = textureId;

    // Simpan posisi sprite dalam game world
    sprite->x = spriteX;
    sprite->y = spriteY;

    // Tambahkan sprite ke dalam sprite order list
    SPRITE_ORDER[SPRITE_COUNT++] = sprite;
}

void Sprite_Render(Game *game, Player *player) {
    // Sort semua sprite yang sudah dibuat
    _Sprite_Sort();

    for (int i = 0; i < SPRITE_COUNT; i++) {
        _Sprite_Render(game, SPRITE_ORDER[i], player);
    }

    // Hapus semua sprite
    SPRITE_COUNT = 0;
}