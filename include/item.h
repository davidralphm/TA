#ifndef __ITEM_H__
#define __ITEM_H__

#include "player.h"
#include "game.h"

// Jumlah penambahan health ketika mengambil health pickup
#define ITEM_HEALTH_PICKUP_VALUE 20

// Jumlah penambahan fireball ketika mengambil fireball pickup
#define ITEM_FIREBALL_PICKUP_VALUE 10

// Jumlah perubahan waktu ketika mengambil time bonus
#define ITEM_TIME_BONUS_PICKUP_VALUE 30

// Timer untuk efek speed boost
#define ITEM_SPEED_BOOST_TIMER 30

// Timer untuk efek x - ray
#define ITEM_XRAY_TIMER 30

// Timer untuk efek slowness
#define ITEM_SLOWNESS_TIMER 15

// Timer untuk efek blindness
#define ITEM_BLINDNESS_TIMER 20

// Tipe - tipe item dalam game
typedef enum item_type_e {
    ITEM_TYPE_TREASURE,
    ITEM_TYPE_HEALTH,
    ITEM_TYPE_LIFE,
    ITEM_TYPE_FIREBALL,
    ITEM_TYPE_TIME,
    ITEM_TYPE_SPEED,
    ITEM_TYPE_XRAY,
    ITEM_TYPE_SLOWNESS,
    ITEM_TYPE_BLINDNESS,
    ITEM_TYPE_TELEPORT,
} ItemType;

// Tipe - tipe pergerakan item dalam game
typedef enum item_movement_e {
    ITEM_MOVEMENT_NONE,
    ITEM_MOVEMENT_X,
    ITEM_MOVEMENT_Y,
} ItemMovement;

// Struct item, benda yang dapat diambil
// oleh pemain ketika disentuh oleh pemain
typedef struct item_t {
    double x, y;
    unsigned char textureId;
    unsigned char deleted;

    ItemMovement movement;
    char moveDir;

    void (*callback)(Game *game);
} Item;

// Fungsi untuk membuat item baru
Item* Item_New(double x, double y, ItemType itemType);

// Fungsi untuk menghapus item
void Item_Free(Item *item);

// Fungsi untuk menggambar semua item dalam game world
void Item_Render(Game *game);

// Fungsi untuk mengupdate semua item dalam game
void Item_Update(Game *game);

// Fungsi untuk menghapus semua item
void Item_DeleteAll(void);

#endif