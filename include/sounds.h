#ifndef __SOUNDS_H__
#define __SOUNDS_H__

#include "player.h"

// Nama - nama dari sound effect yang digunakan dalam game
typedef enum sound_effect_name_e {
    SOUND_EFFECT_BUTTON,
    SOUND_EFFECT_FIREBALL,
    SOUND_EFFECT_EXPLOSION,
    SOUND_EFFECT_PICKUP,
    SOUND_EFFECT_POWERUP,
    SOUND_EFFECT_TRAP,
    SOUND_EFFECT_ZOMBIE_1,
    SOUND_EFFECT_ZOMBIE_2,
    SOUND_EFFECT_ZOMBIE_3,
} SoundEffectName;

// Fungsi untuk loading semua sound effect yang digunakan dalam game
int Sounds_Load(void);

// Fungsi untuk menghapus semua sound effect yang diload
void Sounds_Quit(void);

// Fungsi untuk memutar sebuah sound effect
int Sounds_Play(SoundEffectName name);

// Fungsi untuk memutar sebuah sound effect dengan volume
// yang semakin kecil jika sound berada lebih jauh dari player
void Sounds_Play3D(SoundEffectName name, double soundX, double soundY, Player *player);

#endif