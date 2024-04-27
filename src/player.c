#include "player.h"
#include "fireball.h"

#include <SDL2/SDL.h>
#include <math.h>

#include "map.h"
#include "raycaster.h"
#include "game.h"
#include "sounds.h"

// Sensitivity untuk mouse look
#define PLAYER_MOUSE_LOOK_SENSITIVITY 0.25

// Pengali pergerakan player
#define PLAYER_MOVEMENT_MUL 0.1

// Collision offset untuk pergerakan player
#define PLAYER_COLL_OFFSET 0.2

// Posisi awal pemain dalam labirin
#define PLAYER_STARTING_POSITION 1.5

// Fungsi untuk mengupdate pergerakan player
static void _Player_UpdateMovement(Player *player) {
    // Check input keyboard
    const unsigned char *keystate = SDL_GetKeyboardState(NULL);

    // Mouse look
    int mouseX;
    SDL_GetMouseState(&mouseX, NULL);

    player->angleDegrees -= ((double) (mouseX - WINDOW_W / 2)) * PLAYER_MOUSE_LOOK_SENSITIVITY;
    player->angle = player->angleDegrees / 180.0 * M_PI;

    // Hitung player direction x dan y
    double dirX = cos(player->angle);
    double dirY = sin(player->angle);

    double playerDirX = dirX * player->speed;
    double playerDirY = dirY * player->speed;

    if (keystate[SDL_SCANCODE_S]) {
        playerDirX *= -1;
        playerDirY *= -1;
    } else if (!keystate[SDL_SCANCODE_W]) {
        playerDirX = playerDirY = 0;
    }

    // Hitung player strafe direction x dan y
    double strafeDirX = cos(player->angle - M_PI / 2.0) * player->speed;
    double strafeDirY = sin(player->angle - M_PI / 2.0) * player->speed;

    if (keystate[SDL_SCANCODE_A]) {
        strafeDirX *= -1;
        strafeDirY *= -1;
    } else if (!keystate[SDL_SCANCODE_D]) {
        strafeDirX = strafeDirY = 0;
    }

    playerDirX += strafeDirX;
    playerDirY += strafeDirY;

    // Kalikan player direction x dan y dengan movement multiplier
    playerDirX *= PLAYER_MOVEMENT_MUL;
    playerDirY *= PLAYER_MOVEMENT_MUL;

    // Collision detection pada X axis
    double newX = player->x + playerDirX;
    double collisionDistanceX = (playerDirX < 0.0) ? -PLAYER_COLL_OFFSET : PLAYER_COLL_OFFSET;

    if (
        Map_Get((unsigned) (newX + collisionDistanceX), (unsigned) (player->y - PLAYER_COLL_OFFSET))->isObstacle == 0 &&
        Map_Get((unsigned) (newX + collisionDistanceX), (unsigned) (player->y + PLAYER_COLL_OFFSET))->isObstacle == 0
    ) player->x = newX;

    // Collision detection pada Y axis
    double newY = player->y - playerDirY;
    double collisionDistanceY = (playerDirY < 0.0) ? PLAYER_COLL_OFFSET : -PLAYER_COLL_OFFSET;

    if (
        Map_Get((unsigned) (player->x - PLAYER_COLL_OFFSET), (unsigned) (newY + collisionDistanceY))->isObstacle == 0 &&
        Map_Get((unsigned) (player->x + PLAYER_COLL_OFFSET), (unsigned) (newY + collisionDistanceY))->isObstacle == 0
    ) player->y = newY;
}

// Fungsi untuk mengupdate timer effects
static void _Player_UpdateTimer(Player *player) {
    // Timer untuk efek speed (speed boost dan slowness)
    if (player->speedTimer)
        player->speedTimer--;
    else
        player->speed = PLAYER_SPEED_NORMAL;

    // Timer untuk efek vision (x - ray dan blindness)
    if (player->visionTimer)
        player->visionTimer--;
    else {
        player->fogDist = PLAYER_FOG_DIST_NORMAL;
        player->xray = 0;
    }
}

// Fungsi untuk menembakkan fireball
static void _Player_ShootFireball(Player *player, GameStats *gameStats) {
    if (player->mouseClick && player->fireball > 0 && !player->isPacifist) {
        double dirX = cos(player->angle);
        double dirY = sin(player->angle);

        Fireball_New(player->x, player->y, dirX, -dirY, 0, gameStats);
        player->fireball--;

        Sounds_Play(SOUND_EFFECT_FIREBALL);
    }
}

void Player_Init(Player *player, double x, double y, double angle, double fov) {
    if (!player)
        return;
    
    player->x = x;
    player->y = y;
    player->angle = angle;

    player->camFOV = fov;
    player->camPlaneDist = (double) VIEWPORT_HW / tan(player->camFOV / 2.0);

    player->speed = PLAYER_SPEED_NORMAL;
    player->fogDist = PLAYER_FOG_DIST_NORMAL;
    
    player->health = PLAYER_STARTING_HEALTH;
    player->lives = PLAYER_STARTING_LIVES;
    player->score = 0;

    player->xray = 0;
    player->fireball = PLAYER_STARTING_FIREBALL;

    player->speedTimer = 0;
    player->visionTimer = 0;
}

void Player_SetPosition(Player *player) {
    player->x = PLAYER_STARTING_POSITION;
    player->y = PLAYER_STARTING_POSITION;
    player->angle = 0.0;

    player->xray = 0;
    player->speedTimer = 0;
    player->visionTimer = 0;
}

void Player_ResetStats(Player *player) {
    player->speed = PLAYER_SPEED_NORMAL;
    player->fogDist = PLAYER_FOG_DIST_NORMAL;
    
    player->health = PLAYER_STARTING_HEALTH;
    player->lives = PLAYER_STARTING_LIVES;
    player->score = 0;

    player->xray = 0;
    player->fireball = PLAYER_STARTING_FIREBALL;

    player->speedTimer = 0;
    player->visionTimer = 0;
}

void Player_Update(Player *player, double damageMultiplier, GameStats *gameStats) {
    // Update pergerakan pemain
    _Player_UpdateMovement(player);

    // Update timer - timer untuk efek
    _Player_UpdateTimer(player);

    // Jika player menekan left mouse button, tembakkan sebuah fireball
    // jika jumlah fireball yang dimiliki player > 0 dan mode game bukan pacifist
    _Player_ShootFireball(player, gameStats);

    // Cek jika pemain terkena fireball milik enemy
    Fireball_CheckPlayerHit(player, damageMultiplier, gameStats);
}