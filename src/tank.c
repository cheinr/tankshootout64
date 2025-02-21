#include <stdlib.h>
#include <libdragon.h>
#include <math.h>

#include "tank.h"


#define ADJUSTED_TANK_SPRITE_SIZE 74
#define TANK_SPRITE_TRIMMED_PIXELS_TOP_LEFT 10

#define TANK_SPRITE_CENTER_OFFSET (ADJUSTED_TANK_SPRITE_SIZE / 2)

#define TANK_BODY_SIZE 42

#define TANK_MIN_X 0.0 + (TANK_BODY_SIZE/2)
#define TANK_MAX_X 320.0 - (TANK_BODY_SIZE/2)
#define TANK_MIN_Y 0.0 + (TANK_BODY_SIZE/2)
#define TANK_MAX_Y 240.0 - (TANK_BODY_SIZE/2)

tank_t *tank_init(uint32_t entityId, float xPosition, float yPosition) {

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

  tank->physicsEntity.entityId = entityId;
  tank->physicsEntity.position.x = xPosition;
  tank->physicsEntity.position.y = yPosition;
  tank->physicsEntity.radius = TANK_BODY_SIZE / 2.0;
  tank->physicsEntity.rotation = 0.0;
  tank->physicsEntity.rotationDelta = 0.0;
  tank->physicsEntity.speed = 0.0;

  return tank;
}

void tank_free(tank_t *tank) {
  free(tank->sprite);
  free(tank);
}

void tank_draw(tank_t *tank) {

  int tankWidth = tank->sprite->width/9;
  int hSlicesPerSprite = tank->sprite->hslices/9; // TODO
  int degrees = (int) tank->physicsEntity.rotation;

  int spriteStart = (degrees % 90) * hSlicesPerSprite;
  int reverseDrawOrder = 0;

  int shiftX;
  int shiftY;
  int mirror;
  if (degrees < 90) {
    mirror = MIRROR_DISABLED;
    shiftX = TANK_SPRITE_TRIMMED_PIXELS_TOP_LEFT;
    shiftY = TANK_SPRITE_TRIMMED_PIXELS_TOP_LEFT;
  } else if (degrees < 180) {
    spriteStart = (90 - (degrees % 90) - 1) * hSlicesPerSprite;
    reverseDrawOrder = 1;
    mirror = MIRROR_X;
    shiftX = 0;
    shiftY = TANK_SPRITE_TRIMMED_PIXELS_TOP_LEFT;
  } else if (degrees < 270) {
    mirror = MIRROR_XY;
    reverseDrawOrder = 1;
    shiftX = 0;
    shiftY = 0;
  } else {
    spriteStart = (90 - (degrees % 90) - 1) * hSlicesPerSprite;
    mirror = MIRROR_Y;
    shiftX = TANK_SPRITE_TRIMMED_PIXELS_TOP_LEFT;
    shiftY = 0;
  }

  int i;
  for (i = 0; i < hSlicesPerSprite; i++) {

    int stride = reverseDrawOrder
      ? spriteStart + (hSlicesPerSprite - i - 1)
      : spriteStart + i;

    rdp_load_texture_stride(0, 0, mirror, tank->sprite, stride);

    rdp_sync(SYNC_PIPE);
    int x = (int) tank->physicsEntity.position.x;
    int y = (int) tank->physicsEntity.position.y;

    rdp_draw_sprite(0, x + ((tankWidth/hSlicesPerSprite)*i) + shiftX - TANK_SPRITE_CENTER_OFFSET, y + shiftY - TANK_SPRITE_CENTER_OFFSET, mirror);

  }
}

void tank_tick(tank_t *tank, uint32_t animCounter, const struct SI_condat *gamepad) {

  int rotationAdjustment = gamepad->x/10;
  int speed = gamepad->y/10;

  tank->physicsEntity.rotationDelta = rotationAdjustment;
  tank->physicsEntity.speed = speed;
}
