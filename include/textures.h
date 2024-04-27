#ifndef __TEXTURES_H__
#define __TEXTURES_H__

// Ukuran tiap texture
#define TEXTURE_SIZE 64

// Mask untuk texture size (digunakan untuk modulo)
#define TEXTURE_SIZE_MASK (TEXTURE_SIZE - 1)

// Enum berisi ID tiap texture
typedef enum texture_id_e {
    TEXTURE_ID_WALL_1,
    TEXTURE_ID_WALL_2,
    TEXTURE_ID_WALL_3,
    TEXTURE_ID_WALL_4,
    TEXTURE_ID_WALL_5,
    TEXTURE_ID_WALL_6,
    TEXTURE_ID_WALL_7,
    TEXTURE_ID_WALL_8,
    TEXTURE_ID_TREASURE,
    TEXTURE_ID_HEALTH,
    TEXTURE_ID_LIFE,
    TEXTURE_ID_FIREBALL,
    TEXTURE_ID_TIME_BONUS,
    TEXTURE_ID_SPEED_BOOST,
    TEXTURE_ID_XRAY,
    TEXTURE_ID_TRAP,
    TEXTURE_ID_ENEMY_FRAME_1,
    TEXTURE_ID_ENEMY_FRAME_2,
    TEXTURE_ID_ENEMY_FRAME_3,
    TEXTURE_ID_ENEMY_FRAME_4,
    TEXTURE_ID_ENEMY_FRAME_5,
    TEXTURE_ID_ENEMY_FRAME_6,
    TEXTURE_ID_BARREL_1,
    TEXTURE_ID_BARREL_2,
    TEXTURE_ID_BOXES,
    TEXTURE_ID_CHAIR_1,
    TEXTURE_ID_CHAIR_2,
    TEXTURE_ID_LAMP,
    TEXTURE_ID_BONES,
    TEXTURE_ID_TABLE,
    TEXTURE_ID_FINISH,
    TEXTURE_ID_TORCH_FRAME_1,
    TEXTURE_ID_TORCH_FRAME_2,
    TEXTURE_ID_TORCH_FRAME_3,
    TEXTURE_ID_TORCH_FRAME_4,
    TEXTURE_ID_FIREBALL_FRAME_1,
    TEXTURE_ID_FIREBALL_FRAME_2,
    TEXTURE_ID_FIREBALL_FRAME_3,
    TEXTURE_ID_FIREBALL_FRAME_4,
} TextureID;

// Fungsi untuk load semua texture yang digunakan dalam game
int Textures_Init(void);

// Fungsi untuk mengambil pixel dari sebuah texture
unsigned Textures_Get(unsigned x, unsigned y);

// Fungsi untuk menghapus semua texture
void Textures_Quit(void);

#endif