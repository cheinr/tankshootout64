#include <stdlib.h>
#include <libdragon.h>
#include <math.h>

#include "tank.h"


#define ADJUSTED_TANK_SPRITE_SIZE 74
#define TANK_SPRITE_CENTER_OFFSET (ADJUSTED_TANK_SPRITE_SIZE / 2)

#define TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES 5 //(74-64)/2
#define TANK_BARREL_SPRITE_TRIMMED_PIXELS_TOP_LEFT 31
#define TANK_BARREL_SPRITE_TRIMMED_PIXELS_BOTTOM_RIGHT 11

#define TANK_BODY_SIZE 39

tank_t *tank_init(uint32_t entityId, float xPosition, float yPosition) {

  tank_t *tank = malloc(sizeof(tank_t));

  int fp = dfs_open("/redtankbody64.sprite");
  sprite_t *redtank = malloc(dfs_size(fp));
  dfs_read(redtank, 1, dfs_size(fp), fp);
  dfs_close(fp);

  tank->bodySprite = redtank;

  fp = dfs_open("/redtankbarrel32shifted31.sprite");
  sprite_t *barrelSprite = malloc(dfs_size(fp));
  dfs_read(barrelSprite, 1, dfs_size(fp), fp);
  dfs_close(fp);

  tank->barrelSprite = barrelSprite;

  tank->physicsEntity.entityId = entityId;
  tank->physicsEntity.position.x = xPosition;
  tank->physicsEntity.position.y = yPosition;
  tank->physicsEntity.radius = TANK_BODY_SIZE / 2.0;
  tank->physicsEntity.rotation = 0.0;
  tank->physicsEntity.rotationDelta = 0.0;
  tank->physicsEntity.speed = 0.0;
  tank->physicsEntity.weight = 10.0;

  return tank;
}

void tank_free(tank_t *tank) {
  free(tank->bodySprite);
  free(tank->barrelSprite);
  free(tank);
}

static void tank_draw_sprite(tank_t *tank, sprite_t *sprite,
                             uint32_t spriteOffsetLeft,
                             uint32_t spriteOffsetRight,
                             uint32_t spriteOffsetTop,
                             uint32_t spriteOffsetBottom) {

  int tankWidth = sprite->width/9;
  int hSlicesPerSprite = sprite->hslices/9; // TODO
  int degrees = (int) tank->physicsEntity.rotation;

  int spriteStart = (degrees % 90) * hSlicesPerSprite;

  int reverseDrawOrder = 0;
  int shiftX;
  int shiftY;
  int mirror;
  if (degrees < 90) {
    mirror = MIRROR_DISABLED;
    shiftX = spriteOffsetLeft;
    shiftY = spriteOffsetTop;
  } else if (degrees < 180) {
    spriteStart = (90 - (degrees % 90) - 1) * hSlicesPerSprite;
    reverseDrawOrder = 1;
    mirror = MIRROR_X;
    shiftX = spriteOffsetRight;
    shiftY = spriteOffsetTop;
  } else if (degrees < 270) {
    mirror = MIRROR_XY;
    reverseDrawOrder = 1;
    shiftX = spriteOffsetRight;
    shiftY = spriteOffsetBottom;
  } else {
    spriteStart = (90 - (degrees % 90) - 1) * hSlicesPerSprite;
    mirror = MIRROR_Y;
    shiftX = spriteOffsetLeft;
    shiftY = spriteOffsetRight;
  }

  int i;
  for (i = 0; i < hSlicesPerSprite; i++) {

    int stride = reverseDrawOrder
      ? spriteStart + (hSlicesPerSprite - i - 1)
      : spriteStart + i;

    rdp_load_texture_stride(0, 0, mirror, sprite, stride);

    rdp_sync(SYNC_PIPE);
    int x = (int) tank->physicsEntity.position.x;
    int y = (int) tank->physicsEntity.position.y;

    rdp_draw_sprite(0, x + shiftX + ((tankWidth/hSlicesPerSprite)*i) - TANK_SPRITE_CENTER_OFFSET, y + shiftY - TANK_SPRITE_CENTER_OFFSET, mirror);
  }
}

void tank_draw_body(tank_t *tank) {

  tank_draw_sprite(tank,
                   tank->bodySprite,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES);
}

void tank_draw_barrel(tank_t *tank) {

  tank_draw_sprite(tank,
                   tank->barrelSprite,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_TOP_LEFT,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_BOTTOM_RIGHT,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_TOP_LEFT,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_BOTTOM_RIGHT);
}



void tank_tick(tank_t *tank, uint32_t animCounter, const struct SI_condat *gamepad) {

  int rotationAdjustment = gamepad->x/10;
  int speed = gamepad->y/10;

  tank->physicsEntity.rotationDelta = rotationAdjustment;
  tank->physicsEntity.speed = speed;
}
