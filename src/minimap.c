#include "minimap.h"
#include "raycaster.h"

#include <math.h>

// Constant untuk ukuran cell dalam mini map
#define MINIMAP_CELL_SIZE 10

// Constant untuk ukuran objek dalam mini map
#define MINIMAP_OBJECT_SIZE 4

// Constant untuk jumlah objek maksimum dalam minimap
#define MINIMAP_OBJECT_COUNT_MAX 128

// Radius cell untuk minimap
#define MINIMAP_RADIUS 10

// Array objek minimap
static MinimapObject MINIMAP_OBJECT[MINIMAP_OBJECT_COUNT_MAX];

// Jumlah objek minimap
static unsigned MINIMAP_OBJECT_COUNT = 0;

// Fungsi untuk menampilkan player pada minimap
void _Minimap_RenderPlayer(Game *game) {
    // Tampilkan arah player pada minimap (warna merah)
    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);

    int offset = (MINIMAP_RADIUS / 2) * MINIMAP_CELL_SIZE;

    SDL_RenderDrawLine(
        game->renderer,
        offset + MINIMAP_OBJECT_SIZE / 2,
        offset + MINIMAP_OBJECT_SIZE / 2,
        offset + MINIMAP_OBJECT_SIZE / 2 + cos(game->globals.player.angle) * MINIMAP_CELL_SIZE,
        offset + MINIMAP_OBJECT_SIZE / 2 - sin(game->globals.player.angle) * MINIMAP_CELL_SIZE
    );

    // Tampilkan player pada minimap (warna hijau)
    SDL_Rect playerRect = {
        offset,
        offset,
        MINIMAP_OBJECT_SIZE,
        MINIMAP_OBJECT_SIZE,
    };

    SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(game->renderer, &playerRect);
}

// Fungsi untuk menampilkan tembok - tembok yang sudah terlihat pada minimap
void _Minimap_RenderWalls(Game *game) {
    // Tampilkan tembok - tembok yang sudah terlihat pada minimap
    SDL_Rect mmRect = { 0, 0, MINIMAP_CELL_SIZE - 2, MINIMAP_CELL_SIZE - 2 };

    int minimapOffset = MINIMAP_RADIUS / 2 * MINIMAP_CELL_SIZE;

    int offsetX = -game->globals.player.x * MINIMAP_CELL_SIZE;
    int offsetY = -game->globals.player.y * MINIMAP_CELL_SIZE;

    int playerCellX = (int) game->globals.player.x;
    int playerCellY = (int) game->globals.player.y;

    int x1 = playerCellX - MINIMAP_RADIUS / 2;
    int y1 = playerCellY - MINIMAP_RADIUS / 2;

    int x2 = x1 + MINIMAP_RADIUS + 1;
    int y2 = y1 + MINIMAP_RADIUS + 1;

    x1 = SDL_clamp(x1, 0, game->settings->mazeSize - 1);
    y1 = SDL_clamp(y1, 0, game->settings->mazeSize - 1);
    x2 = SDL_clamp(x2, 0, game->settings->mazeSize - 1);
    y2 = SDL_clamp(y2, 0, game->settings->mazeSize - 1);

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 255, 255);

    for (int i = y1; i <= y2; i++) {
        for (int j = x1; j <= x2; j++) {
            if (Map_Get(j, i)->isSeen) {
                mmRect.x = j * MINIMAP_CELL_SIZE + offsetX + minimapOffset + 1;
                mmRect.y = i * MINIMAP_CELL_SIZE + offsetY + minimapOffset + 1;

                SDL_RenderFillRect(game->renderer, &mmRect);
            }
        }
    }
}

// Fungsi untuk menampilkan objek - objek game dalam minimap
void _Minimap_RenderObjects(Game *game) {
    SDL_Rect rect = {
        0,
        0,
        MINIMAP_OBJECT_SIZE,
        MINIMAP_OBJECT_SIZE
    };

    int minimapOffset = MINIMAP_RADIUS / 2 * MINIMAP_CELL_SIZE;

    int offsetX = -game->globals.player.x * MINIMAP_CELL_SIZE;
    int offsetY = -game->globals.player.y * MINIMAP_CELL_SIZE;

    for (int i = 0; i < MINIMAP_OBJECT_COUNT; i++) {
        MinimapObject *object = &MINIMAP_OBJECT[i];

        rect.x = object->x * MINIMAP_CELL_SIZE - 2 + offsetX + minimapOffset;
        rect.y = object->y * MINIMAP_CELL_SIZE - 2 + offsetY + minimapOffset;

        SDL_SetRenderDrawColor(game->renderer, object->r, object->g, object->b, 255);
        SDL_RenderFillRect(game->renderer, &rect);
    }
}

void Minimap_AddObject(Game *game, double x, double y, unsigned char r, unsigned char g, unsigned char b) {
    if (MINIMAP_OBJECT_COUNT >= MINIMAP_OBJECT_COUNT_MAX) return;

    MinimapObject *object = &MINIMAP_OBJECT[MINIMAP_OBJECT_COUNT++];

    object->x = x;
    object->y = y;

    object->r = r;
    object->g = g;
    object->b = b;
}

void Minimap_Render(Game *game) {
    // Minimap viewport
    SDL_Rect mmViewport = {
        WINDOW_W - MINIMAP_RADIUS * MINIMAP_CELL_SIZE,
        0,
        MINIMAP_RADIUS * MINIMAP_CELL_SIZE,
        MINIMAP_RADIUS * MINIMAP_CELL_SIZE,
    };

    SDL_RenderSetViewport(game->renderer, &mmViewport);

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(game->renderer, NULL);

    _Minimap_RenderPlayer(game);
    _Minimap_RenderWalls(game);
    _Minimap_RenderObjects(game);

    SDL_RenderSetViewport(game->renderer, NULL);

    MINIMAP_OBJECT_COUNT = 0;
}