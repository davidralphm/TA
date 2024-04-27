#include "map.h"
#include "item.h"
#include "teleportexit.h"
#include "decoration.h"
#include "enemyspawner.h"
#include "enemy.h"
#include "fireball.h"

#include <stdio.h>
#include <math.h>

// Array map yang digunakan pada game
static MapCell MAP[MAP_SIZE][MAP_SIZE];

// Ukuran map yang digunakan
static unsigned MAP_CURRENT_SIZE;

// Fungsi untuk membuat titik keluar labirin
static void _Map_CreateExit(void) {
    // Inisialisasi exit cell
    // Exit cell berada pada cell kosong paling ujung kanan
    // bawah labirin
    double cellX = MAP_CURRENT_SIZE - 1;
    double cellY = MAP_CURRENT_SIZE - 1;

    MapCell *exitCell = Map_Get(cellX, cellY);

    exitCell->isWall = 0;
    exitCell->isObstacle = 0;
    exitCell->isExit = 1;
    exitCell->isSeen = 0;
    exitCell->textureId = 0;

    // Buat tanda exit
    Decoration_New(cellX + 0.5, cellY + 0.5, 1.0, DECORATION_FLAG, 0);
}

int Map_Init(Game *game) {
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            if (i == 0 || j == 0) {
                MAP[i][j].isWall = 1;
                MAP[i][j].isObstacle = 1;
                MAP[i][j].isExit = 0;
                MAP[i][j].isSeen = 0;
                MAP[i][j].textureId = 0;
            }
        }
    }

    // Load map
    char mapFilename[64];

    const char *difficultyNames[] = {
        "EASY",
        "MEDIUM",
        "HARD",
    };

    snprintf(mapFilename, 64, "./map/%s%d.txt", difficultyNames[game->globals.difficulty], game->globals.currentLevel);

    // Simpan ukuran map
    MAP_CURRENT_SIZE = game->settings->mazeSize;

    if (!Map_Load(game, mapFilename))
        return 0;
    
    // Buat titik keluar labirin
    _Map_CreateExit();

    return 1;
}

MapCell* Map_Get(unsigned x, unsigned y) {
    return &MAP[y % MAP_CURRENT_SIZE][x % MAP_CURRENT_SIZE];
}

int Map_Load(Game *game, const char *file) {
    if (file == NULL)
        return 0;

    FILE *mapFile = fopen(file, "rb");

    if (mapFile == NULL) {
        printf("Gagal loading map '%s'\n", file);

        return 0;
    }

    // Reset tembok - tembok map
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            MapCell *cell = Map_Get(j, i);

            cell->isWall = 0;
            cell->isObstacle = 0;
            cell->isExit = 0;
            cell->isSeen = 0;
            cell->textureId = 0;
        }
    }
    
    // Hapus semua objek
    Decoration_DeleteAll();
    Enemy_DeleteAll();
    EnemySpawner_DeleteAll();
    Fireball_DeleteAll();
    Item_DeleteAll();
    TeleportExit_DeleteAll();
    
    for (int i = 0; i < game->settings->mazeSize; i++) {
        double cellY = (double) i + 0.5;

        for (int j = 0; j < game->settings->mazeSize; j++) {
            unsigned char cell;
            fread(&cell, sizeof(unsigned char), 1, mapFile);

            if (cell == '\r' || cell == '\n') {
                j--;
                continue;
            }

            MapCell *mapCell = Map_Get(j, i);

            double cellX = (double) j + 0.5;

            // Untuk cell tembok
            if (cell >= MAP_CELL_TYPE_WALL_1 && cell <= MAP_CELL_TYPE_WALL_8) {
                mapCell->isWall = 1;
                mapCell->isObstacle = 1;
                mapCell->textureId = cell - MAP_CELL_TYPE_WALL_1;
                mapCell->isExit = 0;
                mapCell->isSeen = 0;
            } else {
                Item *slownessH, *slownessV,
                     *blindnessH, *blindnessV,
                     *teleportH, *teleportV;

                switch (cell) {
                    case MAP_CELL_TYPE_DECORATION_BARREL_1:
                        Decoration_New(cellX, cellY, 0.5, DECORATION_BARREL_1, 1);
                        break;
                    
                    case MAP_CELL_TYPE_DECORATION_BARREL_2:
                        Decoration_New(cellX, cellY, 0.5, DECORATION_BARREL_2, 1);
                        break;
                    
                    case MAP_CELL_TYPE_DECORATION_BOXES:
                        Decoration_New(cellX, cellY, 0.5, DECORATION_BOXES, 1);
                        break;
                    
                    case MAP_CELL_TYPE_DECORATION_CHAIR_1:
                        Decoration_New(cellX, cellY, 0.5, DECORATION_CHAIR_1, 1);
                        break;
                    
                    case MAP_CELL_TYPE_DECORATION_CHAIR_2:
                        Decoration_New(cellX, cellY, 0.5, DECORATION_CHAIR_2, 1);
                        break;
                    
                    case MAP_CELL_TYPE_DECORATION_LAMP:
                        Decoration_New(cellX, cellY, 1.0, DECORATION_TORCH, 1);
                        break;
                    
                    case MAP_CELL_TYPE_DECORATION_BONES:
                        Decoration_New(cellX, cellY, 1.0, DECORATION_BONES, 1);
                        break;
                    
                    case MAP_CELL_TYPE_DECORATION_TABLE:
                        Decoration_New(cellX, cellY, 0.75, DECORATION_TABLE, 1);
                        break;
                    
                    case MAP_CELL_TYPE_ITEM_TREASURE:
                        Item_New(cellX, cellY, ITEM_TYPE_TREASURE);
                        break;
                    
                    case MAP_CELL_TYPE_ITEM_HEALTH:
                        Item_New(cellX, cellY, ITEM_TYPE_HEALTH);
                        break;
                    
                    case MAP_CELL_TYPE_ITEM_LIFE:
                        Item_New(cellX, cellY, ITEM_TYPE_LIFE);
                        break;
                    
                    case MAP_CELL_TYPE_ITEM_FIREBALL:
                        if (game->globals.mode != GAME_MODE_PACIFIST)
                            Item_New(cellX, cellY, ITEM_TYPE_FIREBALL);
                        
                        break;
                    
                    case MAP_CELL_TYPE_ITEM_TIME:
                        Item_New(cellX, cellY, ITEM_TYPE_TIME);
                        break;
                    
                    case MAP_CELL_TYPE_ITEM_SPEED:
                        Item_New(cellX, cellY, ITEM_TYPE_SPEED);
                        break;
                    
                    case MAP_CELL_TYPE_ITEM_XRAY:
                        Item_New(cellX, cellY, ITEM_TYPE_XRAY);
                        break;
                    
                    case MAP_CELL_TYPE_ITEM_SLOWNESS_H:
                        slownessH = Item_New(cellX, cellY, ITEM_TYPE_SLOWNESS);
                        slownessH->movement = ITEM_MOVEMENT_X;
                        slownessH->moveDir = 1;

                        break;
                    
                    case MAP_CELL_TYPE_ITEM_SLOWNESS_V:
                        slownessV = Item_New(cellX, cellY, ITEM_TYPE_SLOWNESS);
                        slownessV->movement = ITEM_MOVEMENT_Y;
                        slownessV->moveDir = 1;

                        break;
                    
                    case MAP_CELL_TYPE_ITEM_BLINDNESS_H:
                        blindnessH = Item_New(cellX, cellY, ITEM_TYPE_BLINDNESS);
                        blindnessH->movement = ITEM_MOVEMENT_X;
                        blindnessH->moveDir = 1;

                        break;
                    
                    case MAP_CELL_TYPE_ITEM_BLINDNESS_V:
                        blindnessV = Item_New(cellX, cellY, ITEM_TYPE_BLINDNESS);
                        blindnessV->movement = ITEM_MOVEMENT_Y;
                        blindnessV->moveDir = 1;
                        
                        break;
                    
                    case MAP_CELL_TYPE_ITEM_TELEPORT_H:
                        teleportH = Item_New(cellX, cellY, ITEM_TYPE_TELEPORT);
                        teleportH->movement = ITEM_MOVEMENT_X;
                        teleportH->moveDir = 1;

                        break;
                    
                    case MAP_CELL_TYPE_ITEM_TELEPORT_V:
                        teleportV = Item_New(cellX, cellY, ITEM_TYPE_TELEPORT);
                        teleportV->movement = ITEM_MOVEMENT_Y;
                        teleportV->moveDir = 1;
                        
                        break;
                    
                    case MAP_CELL_TYPE_PLAYER_SPAWN:
                        Player_SetPosition(&game->globals.player);

                        if (game->globals.mode == GAME_MODE_PACIFIST)
                            game->globals.player.isPacifist = 1;
                        else
                            game->globals.player.isPacifist = 0;

                        break;
                    
                    case MAP_CELL_TYPE_ENEMY_SPAWN:
                        if (game->globals.mode == GAME_MODE_SURVIVAL) {
                            EnemySpawner_New(cellX, cellY, ENEMY_SPAWNER_DELAY_DEFAULT);
                        } else {
                            Enemy_New(cellX, cellY);
                        }

                        break;
                    
                    case MAP_CELL_TYPE_TELEPORT_EXIT:
                        TeleportExit_New(cellX, cellY);
                        break;

                    // Empty cell
                    default:
                        mapCell->isWall = 0;
                        mapCell->isObstacle = 0;
                        mapCell->isExit = 0;
                        mapCell->isSeen = 0;
                        mapCell->textureId = 0;

                        break;
                }
            }
        }
    }

    fclose(mapFile);
    
    return 1;
}
