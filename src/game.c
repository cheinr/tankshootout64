#include <stdint.h>

#include "game.h"

#define ONE_SECOND_USECONDS 1000000

game_t game;

uint32_t totalRunTimeUSeconds = 0;

game_t* game_init() {
  game.state = STARTING;
  return &game;
}

void game_tick(uint32_t timeDeltaUSeconds) {

  totalRunTimeUSeconds += timeDeltaUSeconds;

  if (game.state == STARTING) {
    if (totalRunTimeUSeconds > 2*ONE_SECOND_USECONDS) {
      game.state = RUNNING;
    }
  }
}
