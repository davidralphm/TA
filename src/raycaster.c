#include "raycaster.h"

#include <SDL2/SDL.h>
#include <math.h>

#include "map.h"
#include "textures.h"
#include "raycaster.h"

// Stencil buffer dan distance buffer untuk melakukan rendering sprite dengan benar

#define STENCIL_BUFFER_SIZE ((VIEWPORT_W * VIEWPORT_H) >> 3)
#define STENCIL_BUFFER_PITCH ((VIEWPORT_W) >> 3)

unsigned char STENCIL_BUFFER[STENCIL_BUFFER_SIZE];

static double WALL_DISTANCE[VIEWPORT_W];

// Fungsi untuk melakukan setting bit pada stencil buffer
void Raycaster_SetStencil(unsigned x, unsigned y, unsigned char bit) {
    x = SDL_clamp(x, 0, VIEWPORT_W - 1);
    y = SDL_clamp(y, 0, VIEWPORT_H - 1);

    unsigned index = y * STENCIL_BUFFER_PITCH + (x >> 3);
    unsigned char byte = STENCIL_BUFFER[index];
    unsigned char shift = (x & 7);

    bit = (bit != 0) << shift;
    unsigned char byteMask = ~(1 << shift);

    byte = (byte & byteMask) | bit;

    STENCIL_BUFFER[index] = byte;
}

// Fungsi untuk mengecek stencil buffer
unsigned char Raycaster_GetStencil(unsigned x, unsigned y) {
    x = SDL_clamp(x, 0, VIEWPORT_W - 1);
    y = SDL_clamp(y, 0, VIEWPORT_H - 1);

    unsigned index = y * STENCIL_BUFFER_PITCH + (x >> 3);

    return (STENCIL_BUFFER[index] >> (x & 7)) & 1;
}

unsigned Raycaster_MultiplyPixel(unsigned pixel, double mul) {
    unsigned char r = (pixel >> 16) & 0xFF;
    unsigned char g = (pixel >> 8) & 0xFF;
    unsigned char b = pixel & 0xFF;

    r *= mul;
    g *= mul;
    b *= mul;

    return (r << 16) | (g << 8) | b;
}

void Raycaster_SetPixel(unsigned *pixels, unsigned x, unsigned y, unsigned pixel) {
    x = SDL_clamp(x, 0, VIEWPORT_W - 1);
    y = SDL_clamp(y, 0, VIEWPORT_H - 1);

    *(pixels + y * VIEWPORT_W + x) = pixel;
}

double Raycaster_CastRay(double x, double y, double angle) {
    // Hitung ray direction x dan y
    double rayDirX = cos(angle);
    double rayDirY = -sin(angle);

    // Hitung delta distance x dan y
    double deltaDistX = fabs(1.0 / rayDirX);
    double deltaDistY = fabs(1.0 / rayDirY);

    // Hitung side distance x dan y
    double sideDistX = (rayDirX < 0.0) ? x - floor(x) : ceil(x) - x;
    double sideDistY = (rayDirY < 0.0) ? ceil(y) - y : y - floor(y);

    sideDistX *= deltaDistX;
    sideDistY *= deltaDistY;

    // Hitung map coordinate x dan y
    int mapX = floor(x);
    int mapY = floor(y);

    // Hitung map step x dan y
    int mapStepX = (rayDirX < 0.0) ? -1 : 1;
    int mapStepY = (rayDirY < 0.0) ? 1 : -1;

    // Cek tembok pada dunia
    int hit = 0;
    int side = 0;

    while (!hit) {
        // Update ray
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += mapStepX;
            side = 0;
        } else {
            sideDistY += deltaDistY;
            mapY += mapStepY;
            side = 1;
        }

        // Cek cell pada posisi ray sekarang
        hit = Map_Get(mapX, mapY)->isWall;
    }

    // Kembalikan jarak ke tembok yang dikenai ray
    return (side == 0) ? sideDistX - deltaDistX : sideDistY - deltaDistY;
}

void Raycaster_Render(unsigned *pixels, Player *player) {
    for (int ray = 0; ray < VIEWPORT_W; ray++) {
        // Hitung sudut dari ray paling kiri layar
        double rayAngle = player->angle + player->camFOV / 2.0 - ((double) ray / (double) VIEWPORT_W) * player->camFOV;

        // Hitung ray direction x dan y
        double rayDirX = cos(rayAngle);
        double rayDirY = sin(rayAngle);

        // Hitung ray cosine untuk menghilangkan efek fisheye
        double rayCos = cos(rayAngle - player->angle);

        // Hitung delta distance x dan y
        double deltaDistX = fabs(1.0 / rayDirX);
        double deltaDistY = fabs(1.0 / rayDirY);

        // Hitung side distance x dan y
        double sideDistX = (rayDirX < 0.0) ? player->x - floor(player->x) : ceil(player->x) - player->x;
        double sideDistY = (rayDirY < 0.0) ? ceil(player->y) - player->y : player->y - floor(player->y);

        sideDistX *= deltaDistX;
        sideDistY *= deltaDistY;

        // Hitung map coordinate x dan y
        int mapX = floor(player->x);
        int mapY = floor(player->y);

        // Hitung map step x dan y
        int mapStepX = (rayDirX < 0.0) ? -1 : 1;
        int mapStepY = (rayDirY < 0.0) ? 1 : -1;

        // Cek tembok pada dunia
        MapCell* hit = NULL;
        int side = 0;

        while (hit == NULL || !hit->isWall) {
            // Update ray
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += mapStepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += mapStepY;
                side = 1;
            }

            // Cek cell pada posisi ray sekarang
            hit = Map_Get(mapX, mapY);
        }

        // Hitung jarak ke tembok yang dikenai ray
        double rayDist = (side == 0) ? sideDistX - deltaDistX : sideDistY - deltaDistY;

        // Hitung texture coordinate x tembok
        unsigned char texX;

        if (side == 0) {
            texX = (int) ((player->y - rayDist * rayDirY) * (double) TEXTURE_SIZE) & TEXTURE_SIZE_MASK;
        } else {
            texX = (int) ((player->x + rayDist * rayDirX) * (double) TEXTURE_SIZE) & TEXTURE_SIZE_MASK;
        }

        // Hilangkan efek fisheye
        rayDist *= rayCos;

        // Simpan jarak tembok
        WALL_DISTANCE[ray] = rayDist;

        // Proyeksikan tembok pada layar
        int y1 = (0.5 - 1.0) / rayDist * player->camPlaneDist + VIEWPORT_HH;
        int y2 = VIEWPORT_H - 1 - y1;

        // Hitung texture y step dan texture y coordinate
        double texYStep = 1.0 / (double) (y2 - y1);
        double texYCoord = (y1 < 0) ? -y1 * texYStep : 0;

        // Clamp koordinate proyeksi
        y1 = SDL_clamp(y1, 0, VIEWPORT_H - 1);
        y2 = SDL_clamp(y2, 0, VIEWPORT_H - 1);

        // Texture ID dari cell
        unsigned char textureId = hit->textureId;

        // Set wall menjadi terlihat
        hit->isSeen = 1;

        // Gambar slice dari tembok
        for (int y = y1; y <= y2; y++) {
            // Hitung texture coordinate y
            unsigned char texY = (int) (texYCoord * TEXTURE_SIZE) & TEXTURE_SIZE_MASK;

            // Update texture coordinate y
            texYCoord += texYStep;

            // Pixel
            unsigned pixel = Textures_Get(texX + textureId * TEXTURE_SIZE, texY);

            // Efek fog untuk pixel
            pixel = Raycaster_MultiplyPixel(pixel, SDL_clamp(1.0 - rayDist / player->fogDist, 0.0, 1.0 - 0.5 * side));

            Raycaster_SetPixel(pixels, ray, y, pixel);

            // Clear stencil buffer
            Raycaster_SetStencil(ray, y, 0);
        }

        // Gambar slice lantai dan langit - langit
        for (int y = y2 + 1; y < VIEWPORT_H; y++) {
            // Hitung jarak lantai ke pemain
            double floorDist = 0.5 * player->camPlaneDist / (double) (y - VIEWPORT_HH);

            unsigned pixel = Raycaster_MultiplyPixel(0x808080, SDL_clamp(1.0 - floorDist / player->fogDist, 0.0, 1.0));

            Raycaster_SetPixel(pixels, ray, y, pixel); // Gambar lantai
            Raycaster_SetPixel(pixels, ray, VIEWPORT_H - 1 - y, pixel); // Gambar langit - langit

            // Clear stencil buffer
            Raycaster_SetStencil(ray, y, 0);
            Raycaster_SetStencil(ray, VIEWPORT_H - 1 - y, 0);
        }
    }
}

double Raycaster_GetWallDist(unsigned ray) {
    return WALL_DISTANCE[SDL_min(ray, VIEWPORT_W - 1)];
}
