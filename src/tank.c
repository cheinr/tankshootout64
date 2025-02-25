#include <stdlib.h>
#include <libdragon.h>
#include <math.h>

#include "tank.h"


#define ADJUSTED_TANK_SPRITE_SIZE 74
#define TANK_SPRITE_CENTER_OFFSET (ADJUSTED_TANK_SPRITE_SIZE / 2)

#define TANK_BODY_SPRITE_HORIZONTAL_SLICES 2
#define TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES 5 //(74-64)/2
#define TANK_BARREL_SPRITE_HORIZONTAL_SLICES 1
#define TANK_BARREL_SPRITE_TRIMMED_PIXELS_TOP_LEFT 31
#define TANK_BARREL_SPRITE_TRIMMED_PIXELS_BOTTOM_RIGHT 11

#define TANK_BODY_SIZE 39

static sprite_t* load_sprite(char* filepath) {
  int fp = dfs_open(filepath);
  sprite_t *sprite = malloc(dfs_size(fp));
  dfs_read(sprite, 1, dfs_size(fp), fp);
  dfs_close(fp);

  return sprite;
}

tank_t *tank_init(uint32_t entityId, float xPosition, float yPosition) {

  tank_t *tank = malloc(sizeof(tank_t));

  for (int i = 0; i < 90; i++) {
    char path[32];
    sprintf(path, "/redtank_body_%04d.png-0.sprite", i+1);

    tank->bodySprites[i*2] = load_sprite(path);

    sprintf(path, "/redtank_body_%04d.png-1.sprite", i+1);

    tank->bodySprites[(i*2) + 1] = load_sprite(path);
  }

  for (int i = 0; i < 90; i++) {
    char path[32];
    sprintf(path, "/redtank_barrel_%04d.sprite", i+1);

    tank->barrelSprites[i] = load_sprite(path);
  }

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
  for (int i = 0; i < 180; i++) {
    free(tank->bodySprites[i]);
  }
  for (int i = 0; i < 90; i++) {
    free(tank->barrelSprites[i]);
  }
  free(tank);
}


static void tank_draw_sprite(tank_t *tank,
                             sprite_t **spriteSet,
                             uint32_t hSlicesPerSprite,
                             uint32_t spriteOffsetLeft,
                             uint32_t spriteOffsetRight,
                             uint32_t spriteOffsetTop,
                             uint32_t spriteOffsetBottom) {

  // All sprites in a set must be the same size; use the first one to get sprite width
  sprite_t* firstSprite = spriteSet[0];
  int totalSpriteWidth = firstSprite->width * hSlicesPerSprite;

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

    sprite_t* sprite = spriteSet[stride];

    rdp_load_texture(0, 0, mirror, sprite);

    rdp_sync(SYNC_PIPE);
    int x = (int) tank->physicsEntity.position.x;
    int y = (int) tank->physicsEntity.position.y;

    rdp_draw_sprite(0, x + shiftX + ((totalSpriteWidth/hSlicesPerSprite)*i) - TANK_SPRITE_CENTER_OFFSET, y + shiftY - TANK_SPRITE_CENTER_OFFSET, mirror);

  }
}

void tank_draw_body(tank_t *tank) {

  tank_draw_sprite(tank,
                   tank->bodySprites,
                   TANK_BODY_SPRITE_HORIZONTAL_SLICES,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES);
}

void tank_draw_barrel(tank_t *tank) {

  tank_draw_sprite(tank,
                   tank->barrelSprites,
                   TANK_BARREL_SPRITE_HORIZONTAL_SLICES,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_TOP_LEFT,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_BOTTOM_RIGHT,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_TOP_LEFT,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_BOTTOM_RIGHT);
}



void tank_tick(tank_t *tank, const struct SI_condat *gamepad) {

  char gamepadx = gamepad->x == -1 ? 0 : gamepad->x;
  char gamepady = gamepad->y == -1 ? 0 : gamepad->y;
  int rotationAdjustment = (gamepadx/85.0) * 90;

  int speed = (gamepady/85.0) * 50;

  tank->physicsEntity.rotationDelta = rotationAdjustment;
  tank->physicsEntity.speed = speed;
}
