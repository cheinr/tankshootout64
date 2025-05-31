#include <stdint.h>

#include "game.h"

#define ONE_SECOND_USECONDS 1000000

game_t game;

uint32_t totalRunTimeUSeconds = 0;

game_t* game_init() {
  game.state = STARTING;
  game.winningPlayer = -1;
  return &game;
}

static char getLastTankAlive(tank_t** tanks) {

  char lastTankStanding = -1;

  for (int i = 0; i < 4; i++) {
    if (tanks[i]->health > 0) {
      if (lastTankStanding == -1) {
        lastTankStanding = i;
      } else {
        return -1;
      }
    }
  }

  return lastTankStanding;
}

void game_tick(uint32_t timeDeltaUSeconds, tank_t** tanks) {

  totalRunTimeUSeconds += timeDeltaUSeconds;

  if (game.state == STARTING) {
    if (totalRunTimeUSeconds > 2*ONE_SECOND_USECONDS) {
      game.state = RUNNING;
    }
  } else if (game.state == RUNNING) {
    char lastTankStanding = getLastTankAlive(tanks);
    if (lastTankStanding != -1) {

      game.winningPlayer = lastTankStanding;
    }
  }
}
