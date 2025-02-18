#include <stdlib.h>
#include <libdragon.h>
#include <math.h>

#include "tank.h"

#define PI 3.14159

tank_t *tank_init(float xPosition, float yPosition) {

  tank_t *tank = malloc(sizeof(tank_t));

  // Get the file handle for our sprite
  int fp = dfs_open("/redtank64shifted10.sprite");
  // Allocate a buffer for our file
  sprite_t *redtank = malloc(dfs_size(fp));

  // Load the sprite into the buffer
  dfs_read(redtank, 1, dfs_size(fp), fp);
  // Reading is complete, close the file
  dfs_close(fp);

  tank->sprite = redtank;
  tank->x = xPosition;
  tank->y = yPosition;
  tank->rotationDegrees = 0;
  tank->dx = 0;
  tank->dy = 0;

  return tank;
}

void tank_free(tank_t *tank) {
  free(tank->sprite);
  free(tank);
}

void tank_draw(tank_t *tank) {

  int tankWidth = tank->sprite->width/9;
  int hSlicesPerSprite = tank->sprite->hslices/9; // TODO
  int degrees = tank->rotationDegrees;

  int spriteStart = (degrees % 90) * hSlicesPerSprite;
  int reverseDrawOrder = 0;

  int shiftX;
  int shiftY;
  int mirror;
  if (degrees < 90) {
    mirror = MIRROR_DISABLED;
    shiftX = 10;
    shiftY = 10;
  } else if (degrees < 180) {
    spriteStart = (90 - (degrees % 90) - 1) * hSlicesPerSprite;
    reverseDrawOrder = 1;
    mirror = MIRROR_X;
    shiftX = 0;
    shiftY = 10;
  } else if (degrees < 270) {
    mirror = MIRROR_XY;
    reverseDrawOrder = 1;
    shiftX = 0;
    shiftY = 0;
  } else {
    spriteStart = (90 - (degrees % 90) - 1) * hSlicesPerSprite;
    mirror = MIRROR_Y;
    shiftX = 10;
    shiftY = 0;
  }

  int i;
  for (i = 0; i < hSlicesPerSprite; i++) {

    int stride = reverseDrawOrder
      ? spriteStart + (hSlicesPerSprite - i - 1)
      : spriteStart + i;

    rdp_load_texture_stride(0, 0, mirror, tank->sprite, stride);

    rdp_sync(SYNC_PIPE);
    int x = (int) tank->x;
    int y = (int) tank->y;

    rdp_draw_sprite(0, x + ((tankWidth/hSlicesPerSprite)*i) + shiftX, y + shiftY, mirror);

  }
}

void tank_tick(tank_t *tank, uint32_t animCounter, const struct SI_condat *gamepad) {

  int rotationAdjustment = gamepad->x/10;

  if (rotationAdjustment < 0) {
    int result = tank->rotationDegrees + rotationAdjustment;
    tank->rotationDegrees = (result < 0) ? 360 + result : result;
  } else {
    tank->rotationDegrees = (tank->rotationDegrees + rotationAdjustment) % 360;
  }

  int speed = gamepad->y/10;

  double rotationRadians = tank->rotationDegrees * (PI/180.0);

  float dx = cos(rotationRadians) * speed;
  float dy = sin(rotationRadians) * speed;

  tank->x += dx;
  tank->y += dy;
}
