#define PROJECTILE_SPRITE_WIDTH 16
#define PROJECTILE_SPRITE_CENTER_OFFSET 8

#include <stdlib.h>
#include <libdragon.h>

#include "projectile.h"

projectile_t* projectile_init(uint32_t entityId, uint32_t parentEntityId, sprite_t* sprite, float x, float y, float rotationDegrees) {

  projectile_t *projectile = malloc(sizeof(projectile_t));

  projectile->sprite = sprite;

  projectile->physicsEntity.type = PROJECTILE;
  projectile->physicsEntity.entityId = entityId;
  projectile->physicsEntity.parentEntityId = parentEntityId;
  projectile->physicsEntity.position.x = x;
  projectile->physicsEntity.position.y = y;
  projectile->physicsEntity.radius = PROJECTILE_SPRITE_CENTER_OFFSET;
  projectile->physicsEntity.rotation = rotationDegrees;
  projectile->physicsEntity.rotationDelta = 0.0;
  projectile->physicsEntity.speed = 225.0;
  projectile->physicsEntity.weight = 0.0;

  return projectile;
}

void projectile_free(projectile_t* projectile) {
  free(projectile);
}

void projectile_draw(projectile_t* projectile) {

  int x = projectile->physicsEntity.position.x;
  int y = projectile->physicsEntity.position.y;

  rdp_load_texture(0, 0, MIRROR_DISABLED, projectile->sprite);
  rdp_sync(SYNC_PIPE);

  rdp_draw_sprite(0, x - PROJECTILE_SPRITE_CENTER_OFFSET, y - PROJECTILE_SPRITE_CENTER_OFFSET, MIRROR_DISABLED);

}
