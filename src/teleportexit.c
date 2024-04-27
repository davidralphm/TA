#include "teleportexit.h"

#include <stdlib.h>

// Jumlah maksimum teleport exit dalam game
#define TELEPORT_EXIT_COUNT_MAX 64

// Array teleport exit
static TeleportExit TELEPORT_EXIT[TELEPORT_EXIT_COUNT_MAX];

// Jumlah teleport exit dalam game
static unsigned TELEPORT_EXIT_COUNT = 0;

TeleportExit* TeleportExit_New(double x, double y) {
    if (TELEPORT_EXIT_COUNT >= TELEPORT_EXIT_COUNT_MAX)
        return NULL;
    
    TeleportExit *te = &TELEPORT_EXIT[TELEPORT_EXIT_COUNT++];

    te->x = x;
    te->y = y;

    return te;
}

TeleportExit* TeleportExit_PickRandom(void) {
    if (TELEPORT_EXIT_COUNT == 0)
        return NULL;
    
    return &TELEPORT_EXIT[rand() % TELEPORT_EXIT_COUNT];
}

void TeleportExit_DeleteAll(void) {
    TELEPORT_EXIT_COUNT = 0;
}