#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <stdint.h>

struct Vector2 {
  float x;
  float y;
};

enum PhysicsEntityType {
  PLAYER,
  PROJECTILE
};

struct physics_entity {
  uint32_t entityId;
  uint32_t parentEntityId;
  enum PhysicsEntityType type;

  struct Vector2 position;
  float rotation;
  float radius;

  float speed;
  float rotationDelta;
  float weight;
  struct Vector2 movementModifier;

  char wasHit;
};

void physics_scene_free();
void physics_scene_init(int worldWidth, int worldHeight);
void physics_scene_add_entity(struct physics_entity* physicsEntity);
struct physics_entity* physics_scene_get_entity(uint32_t entityId);
void physics_scene_remove_entity();
void physics_scene_tick(uint32_t timeDeltaUSeconds);


#endif
