#include <libdragon.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "ui.h"

#define NUM_READY_TEXT_SPRITES 4
#define NUM_GO_TEXT_SPRITES 2
#define USECONDS_PER_SECOND 1000000
#define PI 3.14159

enum GameState lastGameState = -1;

int goTextTimerUSeconds = 0;

char shouldDrawReadyText = 0;
char shouldDrawGoText = 0;

sprite_t* readyTextSprites[NUM_READY_TEXT_SPRITES];
sprite_t* goTextSprites[NUM_GO_TEXT_SPRITES];

uint32_t totalUiTimeMillis = 0;

static sprite_t* load_sprite(char* filepath) {
  int fp = dfs_open(filepath);
  sprite_t *sprite = malloc(dfs_size(fp));
  dfs_read(sprite, 1, dfs_size(fp), fp);
  dfs_close(fp);

  return sprite;
}

void ui_init() {

  int i;
  for (i = 0; i < NUM_READY_TEXT_SPRITES; i++) {
    char path[32];
    sprintf(path, "/readytext128x32-%d.sprite", i);

    readyTextSprites[i] = load_sprite(path);
  }

  for (i = 0; i < NUM_GO_TEXT_SPRITES; i++) {
    char path[32];
    sprintf(path, "/gotext64x32-%d.sprite", i);

    goTextSprites[i] = load_sprite(path);
  }
}

void ui_tick(game_t* game, uint32_t timeDeltaUSeconds) {

  totalUiTimeMillis += timeDeltaUSeconds / 1000;

  if (lastGameState != game->state) {
    lastGameState = game->state;

    shouldDrawReadyText = 0;

    if (game->state == STARTING) {
      shouldDrawReadyText = 1;
    } else if (game->state == RUNNING) {
      goTextTimerUSeconds = USECONDS_PER_SECOND;
    }
  }

  if (goTextTimerUSeconds > 0) {
    goTextTimerUSeconds -= timeDeltaUSeconds;
  }
}


void ui_draw() {

  if (shouldDrawReadyText) {

    float readyTextScale = 1 + .25*sin(((PI*2.0)*((float)totalUiTimeMillis/1000.0)) - PI/2.0);

    for (int i = 0; i < NUM_READY_TEXT_SPRITES; i++) {
      rdp_load_texture(0, 0, MIRROR_DISABLED, readyTextSprites[i]);

      rdp_sync(SYNC_PIPE);

      int startX = 160 - ((128.0*readyTextScale) / 2.0);
      int startY = 120 - (16.0*readyTextScale);

      rdp_draw_sprite_scaled(0,  startX + (32.0 * readyTextScale * i) - i, startY, readyTextScale, readyTextScale, MIRROR_DISABLED);
    }
  }

  if (goTextTimerUSeconds > 0) {

    float goTextDisplayTimeSeconds = (float) (USECONDS_PER_SECOND - goTextTimerUSeconds) / USECONDS_PER_SECOND;
    float goTextScale = 1.5 / (1 + pow(2.71828, -(float)100.0*(goTextDisplayTimeSeconds - .125)));

    for (int i = 0; i < NUM_GO_TEXT_SPRITES; i++) {
      rdp_load_texture(0, 0, MIRROR_DISABLED, goTextSprites[i]);

      rdp_sync(SYNC_PIPE);

      int startX = 160 - ((64.0*goTextScale) / 2.0);
      int startY = 120 - (16.0*goTextScale);

      rdp_draw_sprite_scaled(0,  startX + (32.0 * goTextScale * i) - i, startY, goTextScale, goTextScale, MIRROR_DISABLED);
    }
  }
}
