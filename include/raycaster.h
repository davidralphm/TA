#ifndef __RAYCASTER_H__
#define __RAYCASTER_H__

#include "player.h"
#include "map.h"

// Constants untuk window
#define WINDOW_W 640
#define WINDOW_H 480

#define WINDOW_HW (WINDOW_W >> 1)
#define WINDOW_HH (WINDOW_H >> 1)

// Constants untuk viewport game
#define VIEWPORT_W (WINDOW_W >> 2)
#define VIEWPORT_H (WINDOW_H >> 2)

#define VIEWPORT_HW (VIEWPORT_W >> 1)
#define VIEWPORT_HH (VIEWPORT_H >> 1)

// Fungsi untuk meletakkan pixel pada texture
void Raycaster_SetPixel(unsigned *pixels, unsigned x, unsigned y, unsigned pixel);

// Fungsi untuk mengcast satu ray dari satu titik dengan arah tertentu pada world
// Fungsi ini mengembalikan jarak titik asal ke tembok yang dikenai ray
double Raycaster_CastRay(double x, double y, double angle);

// Fungsi untuk rendering game world
void Raycaster_Render(unsigned *pixels, Player *player);

// Fungsi untuk melakukan setting bit pada stencil buffer
void Raycaster_SetStencil(unsigned x, unsigned y, unsigned char bit);

// Fungsi untuk mengecek stencil buffer
unsigned char Raycaster_GetStencil(unsigned x, unsigned y);

// Fungsi untuk mengecek jarak dari slice tembok untuk ray tertentu
double Raycaster_GetWallDist(unsigned ray);

// Fungsi untuk mengalikan pixel
unsigned Raycaster_MultiplyPixel(unsigned pixel, double mul);

#endif