#ifndef __TANK_H
#define __TANK_H

#include <libdragon.h>
#include "physics.h"

typedef struct tank_s {
  sprite_t *bodySprite;
  sprite_t *barrelSprite;
  int xSlices;
  int ySlices;

  struct physics_entity physicsEntity;
} tank_t;

tank_t *tank_init(uint32_t entityId, float xPosition, float yPosition);
void tank_free(tank_t *tank);
void tank_draw_body(tank_t *tank);
void tank_draw_barrel(tank_t *tank);
void tank_tick(tank_t *tank, const struct SI_condat *gamepad);

#endif
