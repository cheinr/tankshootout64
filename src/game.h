#ifndef __GAME_H__
#define __GAME_H__

#include "tank.h"

enum GameState {
  INITIALIZING,
  STARTING,
  RUNNING,
  ENDED
};

typedef struct game_s {
  enum GameState state;
  char winningPlayer;
} game_t;

game_t* game_init();
void game_start();
void game_tick(uint32_t timeDeltaUSeconds, tank_t** tanks);

#endif
