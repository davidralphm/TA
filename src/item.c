#include "item.h"
#include "sprite.h"
#include "map.h"
#include "game.h"
#include "textures.h"
#include "sounds.h"
#include "teleportexit.h"

#include <stdio.h>
#include <stdlib.h>

// Scale value untuk sprite item pickup
#define ITEM_PICKUP_SCALE 0.5

// Pengali pergerakan item trap
#define ITEM_TRAP_SPEED_MUL 0.05

// Jarak minimum untuk mengambil item
#define ITEM_PICKUP_DIST 0.8

// Jumlah item maksimum
#define ITEM_COUNT_MAX 256

// Array dari item yang ada di dalam game
static Item ITEM[ITEM_COUNT_MAX];

// Jumlah item yang ada di dalam game
static unsigned ITEM_COUNT = 0;

// Fungsi callback untuk item treasure
static void _Item_Callback_Treasure(Game *game) {
    Sounds_Play(SOUND_EFFECT_PICKUP);

    game->globals.player.lives += Game_AddPlayerScore(game, PLAYER_SCORE_TREASURE);

    // Untuk stats
    game->stats.scoreGot += PLAYER_SCORE_TREASURE;
    game->stats.totalScoreGot += PLAYER_SCORE_TREASURE;
}

// Fungsi callback untuk item health
static void _Item_Callback_Health(Game *game) {
    Sounds_Play(SOUND_EFFECT_PICKUP);

    game->globals.player.health += ITEM_HEALTH_PICKUP_VALUE;
    if (game->globals.player.health > PLAYER_MAX_HEALTH) game->globals.player.health = PLAYER_MAX_HEALTH;
}

// Fungsi callback untuk item life
static void _Item_Callback_Life(Game *game) {
    Sounds_Play(SOUND_EFFECT_PICKUP);

    game->globals.player.lives++;
}

// Fungsi callback untuk item fireball
static void _Item_Callback_Fireball(Game *game) {
    Sounds_Play(SOUND_EFFECT_PICKUP);

    game->globals.player.fireball += ITEM_FIREBALL_PICKUP_VALUE;

    // Clamp jumlah fireball player
    game->globals.player.fireball = SDL_min(game->globals.player.fireball, PLAYER_MAX_FIREBALL);
}

// Fungsi callback untuk item time bonus
static void _Item_Callback_Time(Game *game) {
    if (game->globals.mode != GAME_MODE_SURVIVAL) {
        game->globals.timeLeft += ITEM_TIME_BONUS_PICKUP_VALUE;
    } else {
        if (game->globals.timeLeft >= ITEM_TIME_BONUS_PICKUP_VALUE)
            game->globals.timeLeft -= ITEM_TIME_BONUS_PICKUP_VALUE;
        else
            game->globals.timeLeft = 0;
    }

    Sounds_Play(SOUND_EFFECT_POWERUP);
}

// Fungsi callback untuk item speed boost
static void _Item_Callback_Speed(Game *game) {
    game->globals.player.speed = PLAYER_SPEED_SPEED_BOOST;
    game->globals.player.speedTimer = ITEM_SPEED_BOOST_TIMER * 60; // 30 detik

    Sounds_Play(SOUND_EFFECT_POWERUP);
}

// Fungsi callback untuk item X - Ray
static void _Item_Callback_XRay(Game *game) {
    game->globals.player.xray = 1;
    game->globals.player.fogDist = PLAYER_FOG_DIST_XRAY;
    game->globals.player.visionTimer = ITEM_XRAY_TIMER * 60; // 30 detik

    Sounds_Play(SOUND_EFFECT_POWERUP);
}

// Fungsi callback untuk item slowness
static void _Item_Callback_Slowness(Game *game) {
    game->globals.player.speed = PLAYER_SPEED_SLOWNESS;
    game->globals.player.speedTimer = ITEM_SLOWNESS_TIMER * 60; // 15 detik

    Sounds_Play(SOUND_EFFECT_TRAP);
}

// Fungsi callback untuk item blindness
static void _Item_Callback_Blindness(Game *game) {
    game->globals.player.xray = 0;
    game->globals.player.fogDist = PLAYER_FOG_DIST_BLINDNESS;
    game->globals.player.visionTimer = ITEM_BLINDNESS_TIMER * 60; // 20 detik

    Sounds_Play(SOUND_EFFECT_TRAP);
}

// Fungsi callback untuk item teleport
static void _Item_Callback_Teleport(Game *game) {
    Sounds_Play(SOUND_EFFECT_TRAP);

    // Pilih teleport exit secara acak
    TeleportExit *te = TeleportExit_PickRandom();

    if (te != NULL) {
        game->globals.player.x = te->x;
        game->globals.player.y = te->y;
    }
}

// Array fungsi callback untuk setiap tipe item
static void (*ITEM_CALLBACK[])(Game *game) = {
    _Item_Callback_Treasure,
    _Item_Callback_Health,
    _Item_Callback_Life,
    _Item_Callback_Fireball,
    _Item_Callback_Time,
    _Item_Callback_Speed,
    _Item_Callback_XRay,
    _Item_Callback_Slowness,
    _Item_Callback_Blindness,
    _Item_Callback_Teleport,
};

// Fungsi untuk mengupdate satu item
static void _Item_Update(Game *game, Item *item) {
    // Gerakkan item
    if (item->movement == ITEM_MOVEMENT_X) {
        item->x += ITEM_TRAP_SPEED_MUL * item->moveDir * game->settings->trapSpeed;
    } else if (item->movement == ITEM_MOVEMENT_Y) {
        item->y += ITEM_TRAP_SPEED_MUL * item->moveDir * game->settings->trapSpeed;
    }

    // Jika item mengenai tembok, maka item akan memantul
    unsigned mapX = (unsigned) item->x;
    unsigned mapY = (unsigned) item->y;

    if (Map_Get(mapX, mapY)->isObstacle) item->moveDir *= -1;

    // Cek jarak pemain dengan item
    double dist = (item->x - game->globals.player.x) * (item->x - game->globals.player.x);
    dist += (item->y - game->globals.player.y) * (item->y - game->globals.player.y);

    // Jika pemain berada dekat dengan item, item akan diambil oleh pemain
    // dan akan dihapus dari game world
    if (dist < ITEM_PICKUP_DIST) {
        // Jika objek merupakan health item dan health player 100, maka
        // item tidak akan diambil
        if (game->globals.player.health == PLAYER_MAX_HEALTH && item->textureId == TEXTURE_ID_HEALTH)
            return;
        
        // Jika objek merupakan fireball item dan fireball milik player 100, maka
        // item tidak akan diambil
        if (game->globals.player.fireball == PLAYER_MAX_FIREBALL && item->textureId == TEXTURE_ID_FIREBALL)
            return;
            
        item->deleted = 1;

        // Jalankan fungsi callback item
        item->callback(game);

        // Untuk stats
        game->stats.itemsTaken++;
        game->stats.totalItemsTaken++;
    }
}

Item* Item_New(double x, double y, ItemType itemType) {
    if (ITEM_COUNT >= ITEM_COUNT_MAX) return NULL;

    Item *item = &ITEM[ITEM_COUNT++];

    itemType = SDL_clamp(itemType, ITEM_TYPE_TREASURE, ITEM_TYPE_TELEPORT);

    item->x = x;
    item->y = y;
    item->textureId = SDL_clamp(TEXTURE_ID_TREASURE + itemType, TEXTURE_ID_TREASURE, TEXTURE_ID_TRAP);
    item->deleted = 0;
    item->callback = ITEM_CALLBACK[itemType];
    item->movement = ITEM_MOVEMENT_NONE;
    item->moveDir = 0;

    return item;
}

void Item_Free(Item *item) {
    // Hitung index item di dalam array
    unsigned index = item - ITEM;

    if (index >= ITEM_COUNT) return;

    // Replace item pada index item yang dihapus dengan
    // item terakhir yang ada di dalam array
    ITEM_COUNT--;
    ITEM[index] = ITEM[ITEM_COUNT];
}

void Item_Render(Game *game) {
    for (int i = 0; i < ITEM_COUNT; i++) {
        Item *item = &ITEM[i];

        Sprite_Add(&game->globals.player, item->x, item->y, item->textureId, ITEM_PICKUP_SCALE);
    }
}

void Item_Update(Game *game) {
    // Update semua item
    for (int i = 0; i < ITEM_COUNT; i++)
        _Item_Update(game, &ITEM[i]);

    // Hapus item yang diambil pemain
    for (int i = 0; i < ITEM_COUNT; i++) {
        if (ITEM[i].deleted)
            Item_Free(&ITEM[i--]);
    }
}

void Item_DeleteAll(void) {
    ITEM_COUNT = 0;
}
