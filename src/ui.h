
#ifndef __UI_H__
#define __UI_H__

#include "game.h"

void ui_init();
void ui_tick(game_t* game, uint32_t timeDeltaUSeconds);
void ui_draw();

#endif
