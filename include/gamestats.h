#ifndef __GAME_STATS_H__
#define __GAME_STATS_H__

// Tipe yang menyimpan stats game
typedef struct game_stats_t {
    int enemiesKilled, totalEnemiesKilled;
    int itemsTaken, totalItemsTaken;
    int scoreGot, totalScoreGot;
    int fireballsShot, totalFireballsShot;
    int damageDealt, totalDamageDealt;
    int damageReceived, totalDamageReceived;
    int timeSpent, totalTimeSpent;
} GameStats;

#endif