#include "sounds.h"

#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#define SOUND_EFFECT_COUNT 6

// Array sound effect
static Mix_Chunk* SOUND_EFFECT[SOUND_EFFECT_COUNT];

// Array nama - nama sound effect yang diload
static const char *SOUND_EFFECT_NAME[SOUND_EFFECT_COUNT] = {
    "./res/button.wav",
    "./res/fireball.wav",
    "./res/explosion.wav",
    "./res/pickup.wav",
    "./res/powerup.wav",
    "./res/trap.wav",
};

int Sounds_Load(void) {
    for (int i = 0; i < SOUND_EFFECT_COUNT; i++) {
        SOUND_EFFECT[i] = Mix_LoadWAV(SOUND_EFFECT_NAME[i]);

        if (SOUND_EFFECT[i] == NULL) {
            printf("Gagal melakukan loading sound effect '%s'\n", SOUND_EFFECT_NAME[i]);

            return 0;
        } else {
            Mix_VolumeChunk(SOUND_EFFECT[i], 16);
        }
    }

    return 1;
}

void Sounds_Quit(void) {
    for (int i = 0; i < SOUND_EFFECT_COUNT; i++) {
        if (SOUND_EFFECT[i] != NULL) Mix_FreeChunk(SOUND_EFFECT[i]);
    }
}

int Sounds_Play(SoundEffectName name) {
    return Mix_PlayChannel(-1, SOUND_EFFECT[name], 0);
}

void Sounds_Play3D(SoundEffectName name, double soundX, double soundY, Player *player) {
    double dist = (player->x - soundX) * (player->x - soundX);
    dist += (player->y - soundY) * (player->y - soundY);

    int channel = Sounds_Play(name);
    Mix_SetDistance(channel, SDL_clamp((unsigned) dist * 2, 0, 255));
}
