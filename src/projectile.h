#ifndef __PROJECTILE_H
#define __PROJECTILE_H

#include <libdragon.h>
#include "physics.h"

typedef struct projectile_s {
  sprite_t* sprite;
  struct physics_entity physicsEntity;
} projectile_t;

projectile_t *projectile_init(uint32_t entityId, uint32_t parentEntityId, sprite_t* sprite, float xPosition, float yPosition, float rotationDegrees);
void projectile_free(projectile_t *projectile);
void projectile_draw(projectile_t *projectile);

#endif

