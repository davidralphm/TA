#ifndef __MAP_H__
#define __MAP_H__

#include "game.h"

// Constants untuk map dari game
#define MAP_SIZE 32
#define MAP_SIZE_MASK (MAP_SIZE - 1)

// Tipe map cell
typedef struct map_cell_t {
    unsigned char isWall:1,
                  isObstacle:1,
                  isExit:1,
                  isSeen:1;

    unsigned char textureId;
} MapCell;

// Enum untuk tipe - tipe cell ketika loading map
typedef enum map_cell_type_e {
    MAP_CELL_TYPE_WALL_1 = '1',
    MAP_CELL_TYPE_WALL_2 = '2',
    MAP_CELL_TYPE_WALL_3 = '3',
    MAP_CELL_TYPE_WALL_4 = '4',
    MAP_CELL_TYPE_WALL_5 = '5',
    MAP_CELL_TYPE_WALL_6 = '6',
    MAP_CELL_TYPE_WALL_7 = '7',
    MAP_CELL_TYPE_WALL_8 = '8',

    MAP_CELL_TYPE_DECORATION_BARREL_1  = '!',
    MAP_CELL_TYPE_DECORATION_BARREL_2  = '@',
    MAP_CELL_TYPE_DECORATION_BOXES     = '#',
    MAP_CELL_TYPE_DECORATION_CHAIR_1   = '$',
    MAP_CELL_TYPE_DECORATION_CHAIR_2   = '%',
    MAP_CELL_TYPE_DECORATION_LAMP      = '^',
    MAP_CELL_TYPE_DECORATION_BONES     = '&',
    MAP_CELL_TYPE_DECORATION_TABLE     = '*',
    MAP_CELL_TYPE_DECORATION_TORCH     = '(',

    MAP_CELL_TYPE_ITEM_TREASURE        = 'a',
    MAP_CELL_TYPE_ITEM_HEALTH          = 's',
    MAP_CELL_TYPE_ITEM_LIFE            = 'd',
    MAP_CELL_TYPE_ITEM_FIREBALL        = 'f',
    MAP_CELL_TYPE_ITEM_TIME            = 'g',
    MAP_CELL_TYPE_ITEM_SPEED           = 'h',
    MAP_CELL_TYPE_ITEM_XRAY            = 'j',
    MAP_CELL_TYPE_ITEM_SLOWNESS_H      = 'z',
    MAP_CELL_TYPE_ITEM_SLOWNESS_V      = 'x',
    MAP_CELL_TYPE_ITEM_BLINDNESS_H     = 'c',
    MAP_CELL_TYPE_ITEM_BLINDNESS_V     = 'v',
    MAP_CELL_TYPE_ITEM_TELEPORT_H      = 'b',
    MAP_CELL_TYPE_ITEM_TELEPORT_V      = 'n',

    MAP_CELL_TYPE_PLAYER_SPAWN         = 'p',
    MAP_CELL_TYPE_ENEMY_SPAWN          = 'e',
    MAP_CELL_TYPE_TELEPORT_EXIT        = 't',
} MapCellType;

// Fungsi untuk menginisialisasi map
int Map_Init(Game *game);

// Fungsi untuk mengambil cell pada map
MapCell* Map_Get(unsigned x, unsigned y);

// Fungsi untuk loading sebuah map dari file
int Map_Load(Game *game, const char *file);

#endif