#ifndef __TANK_H
#define __TANK_H

#include <libdragon.h>
#include "physics.h"
#include "projectile.h"

typedef struct tank_s {
  // Loading sprites individually seems to be way faster
  // than relying on rdp "_stride" functions for loading
  // from a spritesheet. Went from ~14fps to ~60 with
  // this change.
  sprite_t *bodySprites[180];
  sprite_t *barrelSprites[90];
  int xSlices;
  int ySlices;

  char aWasPressed;

  projectile_t* projectile;
  struct physics_entity physicsEntity;

  int hitCooldownMillis;
  char health;
} tank_t;

tank_t *tank_init(uint32_t entityId, float xPosition, float yPosition, float rotationDegrees);
void tank_free(tank_t *tank);
void tank_draw_body(tank_t *tank);
void tank_draw_barrel(tank_t *tank);
void tank_tick(tank_t *tank, int gamepadConnected, const struct SI_condat *gamepad, uint32_t timeDeltaUSeconds);

#endif
