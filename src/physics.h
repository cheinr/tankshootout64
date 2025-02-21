#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <stdint.h>

struct Vector2 {
  float x;
  float y;
};

struct physics_entity {
  uint32_t entityId;

  struct Vector2 position;
  float rotation;
  float radius;

  float speed;
  float rotationDelta;
};

void physics_scene_init(int worldWidth, int worldHeight);
void physics_scene_add_entity(struct physics_entity* physicsEntity);
void physics_scene_remove_entity();
void physics_scene_tick();


#endif
