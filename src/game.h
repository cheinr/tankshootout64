#ifndef __GAME_H__
#define __GAME_H__

enum GameState {
  STARTING,
  RUNNING,
  ENDED
};

typedef struct game_s {
  enum GameState state;
} game_t;

game_t* game_init();
void game_tick(uint32_t timeDeltaUSeconds);

#endif
