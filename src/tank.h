#ifndef __TANK_H
#define __TANK_H

typedef struct tank_s {
  sprite_t *sprite;
  int xSlices;
  int ySlices;

  int rotationDegrees;
  float x;
  float y;
  float dx;
  float dy;
} tank_t;

tank_t *tank_init(float xPosition, float yPosition);
void tank_free(tank_t *tank);
void tank_draw(tank_t *tank);
void tank_tick(tank_t *tank, uint32_t animCounter);

#endif
