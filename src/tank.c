#include <stdlib.h>
#include <libdragon.h>
#include <math.h>

#include "tank.h"
#include "id.h"


#define ADJUSTED_TANK_SPRITE_SIZE 74
#define TANK_SPRITE_CENTER_OFFSET (ADJUSTED_TANK_SPRITE_SIZE / 2)

#define TANK_BODY_SPRITE_HORIZONTAL_SLICES 2
#define TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES 5 //(74-64)/2
#define TANK_BARREL_SPRITE_HORIZONTAL_SLICES 1
#define TANK_BARREL_SPRITE_TRIMMED_PIXELS_TOP_LEFT 31
#define TANK_BARREL_SPRITE_TRIMMED_PIXELS_BOTTOM_RIGHT 11

#define SHIELD_SPRITE_CENTER_OFFSET_X 6
#define SHIELD_SPRITE_CENTER_OFFSET_Y 7

#define TANK_BODY_SIZE 39

#define PI 3.14159


#define PROJECTILE_SPEED 225.0
#define MAX_PROJECTILE_RANGE 400.0 // (happens to be the length of the screen diagonal)
#define PROJECTILE_FIRE_COOLDOWN_MILLIS ((MAX_PROJECTILE_RANGE / PROJECTILE_SPEED) * 1000)

sprite_t* projectileSprite;
sprite_t* shieldSprites[3];

static sprite_t* load_sprite(char* filepath) {
  int fp = dfs_open(filepath);
  sprite_t *sprite = malloc(dfs_size(fp));
  dfs_read(sprite, 1, dfs_size(fp), fp);
  dfs_close(fp);

  return sprite;
}

tank_t *tank_init(float xPosition, float yPosition, float rotationDegrees) {

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

  projectileSprite = load_sprite("/projectile16.sprite");

  shieldSprites[0] = load_sprite("/shield0.sprite");
  shieldSprites[1] = load_sprite("/shield1.sprite");
  shieldSprites[2] = load_sprite("/shield2.sprite");

  tank->aWasPressed = 0;
  tank->projectile = NULL;
  tank->health = 3;

  tank->physicsEntity.type = PLAYER;
  tank->physicsEntity.entityId = next_entity_id();
  tank->physicsEntity.parentEntityId = UINT32_MAX;
  tank->physicsEntity.position.x = xPosition;
  tank->physicsEntity.position.y = yPosition;
  tank->physicsEntity.radius = TANK_BODY_SIZE / 2.0;
  tank->physicsEntity.rotation = rotationDegrees;
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

static int calculate_blink_frame(tank_t *tank) {

  if (tank->hitCooldownMillis <= 0) {
    return tank->health > 0;
  }

  float timeSeconds = tank->hitCooldownMillis / 1000.0;
  float y = sin(10.0*PI*timeSeconds);

  return y >= 0 ? 1 : 0;
}

void tank_draw_body(tank_t *tank) {

  // Makes the tank 'blink' after it's been hit
  if (calculate_blink_frame(tank) != 1) {
    return;
  }

  tank_draw_sprite(tank,
                   tank->bodySprites,
                   TANK_BODY_SPRITE_HORIZONTAL_SLICES,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES,
                   TANK_BODY_SPRITE_TRIMMED_PIXELS_ALL_SIDES);

  if (tank->projectile != NULL) {
    projectile_draw(tank->projectile);
  }
}

void tank_draw_barrel(tank_t *tank) {

  // Makes the tank 'blink' after it's been hit
  if (calculate_blink_frame(tank) != 1) {
    return;
  }

  tank_draw_sprite(tank,
                   tank->barrelSprites,
                   TANK_BARREL_SPRITE_HORIZONTAL_SLICES,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_TOP_LEFT,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_BOTTOM_RIGHT,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_TOP_LEFT,
                   TANK_BARREL_SPRITE_TRIMMED_PIXELS_BOTTOM_RIGHT);

  if (tank->health > 0) {
    int x = (int) tank->physicsEntity.position.x;
    int y = (int) tank->physicsEntity.position.y;

    int shieldSprite = tank->health - 1;

    rdp_load_texture(0, 0, MIRROR_DISABLED, shieldSprites[shieldSprite]);
    rdp_draw_sprite(0, x - SHIELD_SPRITE_CENTER_OFFSET_X, y - SHIELD_SPRITE_CENTER_OFFSET_Y, MIRROR_DISABLED);
  }
}


void tank_tick(tank_t *tank, int gamepadConnected, const struct SI_condat *gamepad, uint32_t timeDeltaUSeconds) {

  if (tank->hitCooldownMillis > 0) {
    tank->hitCooldownMillis -= (timeDeltaUSeconds / 1000);
  }

  if (tank->fireCooldownMillis > 0) {
    tank->fireCooldownMillis -= (timeDeltaUSeconds / 1000);
  }

  if (tank->physicsEntity.wasHit && tank->hitCooldownMillis <= 0) {
    tank->physicsEntity.wasHit = 0;
    tank->hitCooldownMillis = 1000;

    if (tank->health > 0) {
      tank->health--;

      if (tank->health < 1) {
        physics_scene_remove_entity(tank->physicsEntity.entityId);
      }
    }
  }

  if (!gamepadConnected) {
    tank->physicsEntity.rotationDelta = 0.0;
    tank->physicsEntity.speed = 0.0;
    tank->aWasPressed = 0;
    return;
  }

  char gamepadx = gamepad->x == -1 ? 0 : gamepad->x;
  char gamepady = gamepad->y == -1 ? 0 : gamepad->y;
  int rotationAdjustment = (gamepadx/85.0) * 90;

  int speed = (gamepady/85.0) * 50;

  tank->physicsEntity.rotationDelta = rotationAdjustment;
  tank->physicsEntity.speed = speed;

  if (tank->projectile != NULL) {
    if (tank->fireCooldownMillis <= 0) {
      physics_scene_remove_entity(tank->projectile->physicsEntity.entityId);
    }

    if (physics_scene_get_entity(tank->projectile->physicsEntity.entityId) == NULL) {
      projectile_free(tank->projectile);
      tank->projectile = NULL;
    }
  }

  if (!tank->aWasPressed && gamepad->A && tank->projectile == NULL && tank->fireCooldownMillis <= 0) {

    float projectileStartRadius = (TANK_BODY_SIZE/2) + 8 + 8;
    float tankRotationRadians = tank->physicsEntity.rotation * (PI/180.0);

    float xOffset = cos(tankRotationRadians) * projectileStartRadius;
    float yOffset = sin(tankRotationRadians) * projectileStartRadius;

    projectile_t* projectile = projectile_init(tank->physicsEntity.entityId,
                                               projectileSprite,
                                               tank->physicsEntity.position.x + xOffset,
                                               tank->physicsEntity.position.y + yOffset,
                                               tank->physicsEntity.rotation,
                                               PROJECTILE_SPEED);

    physics_scene_add_entity(&projectile->physicsEntity);

    tank->projectile = projectile;
    tank->fireCooldownMillis = (int) PROJECTILE_FIRE_COOLDOWN_MILLIS;
  }
  tank->aWasPressed = gamepad->A;
}
