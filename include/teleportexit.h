#ifndef __TELEPORT_EXIT_H__
#define __TELEPORT_EXIT_H__

#include "game.h"

// Tipe teleport exit
typedef struct teleport_exit_t {
    double x, y;
} TeleportExit;

// Fungsi untuk membuat teleport exit baru
TeleportExit* TeleportExit_New(double x, double y);

// Fungsi untuk memilih sebuah teleport exit secara acak
TeleportExit* TeleportExit_PickRandom(void);

// Fungsi untuk menghapus semua teleport exit
void TeleportExit_DeleteAll(void);

#endif