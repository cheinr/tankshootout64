#include "physics.h"

#include <stdint.h>
#include <stdlib.h>
#include <libdragon.h>
#include <math.h>

#define EXPECTED_MAX_PHYSICS_ENTITIES 4

struct physics_scene {
  uint32_t sceneWidth;
  uint32_t sceneHeight;

  struct physics_entity** physicsEntities;
  uint32_t physicsEntityCount;
  uint32_t physicsEntityCapacity;
};

struct physics_scene g_physicsScene;

void physics_scene_init(int worldWidth, int worldHeight) {
  g_physicsScene.sceneWidth = worldWidth;
  g_physicsScene.sceneHeight = worldHeight;

  g_physicsScene.physicsEntities = malloc(sizeof(struct physics_entity) * EXPECTED_MAX_PHYSICS_ENTITIES);
  g_physicsScene.physicsEntityCount = 0;
  g_physicsScene.physicsEntityCapacity = EXPECTED_MAX_PHYSICS_ENTITIES;
}

void physics_scene_add_entity(struct physics_entity* physicsEntity) {

  if (g_physicsScene.physicsEntityCount >= g_physicsScene.physicsEntityCapacity) {
    // TODO
    debugf("Failed to add physics entity! Out of capacity!\n");
    return;
  }

  g_physicsScene.physicsEntities[g_physicsScene.physicsEntityCount++] = physicsEntity;
}

void physics_scene_remove_entity() {
  // TODO
}

#define PI 3.14159

static void update_entity_positions() {
  for (int i = 0; i < g_physicsScene.physicsEntityCount; i++) {
    struct physics_entity* physicsEntity = g_physicsScene.physicsEntities[i];

    double rotationRadians = physicsEntity->rotation * (PI/180.0);
    float dx = cos(rotationRadians) * physicsEntity->speed;
    float dy = sin(rotationRadians) * physicsEntity->speed;
    
    float newX = physicsEntity->position.x + dx;
    float newY = physicsEntity->position.y + dy;

    if (newX + physicsEntity->radius >= g_physicsScene.sceneWidth) {
      newX = g_physicsScene.sceneWidth - 1 - physicsEntity->radius;
    } else if (newX - physicsEntity->radius < 0) {
      newX = 0 + physicsEntity->radius;
    }

    if (newY + physicsEntity->radius >= g_physicsScene.sceneHeight) {
      newY = g_physicsScene.sceneHeight - 1 - physicsEntity->radius;
    } else if (newY - physicsEntity->radius < 0) {
      newY = 0 + physicsEntity->radius;
    }

    physicsEntity->position.x = newX;
    physicsEntity->position.y = newY;

    float adjustedRotation = physicsEntity->rotation + physicsEntity->rotationDelta;
    physicsEntity->rotation = (adjustedRotation < 0)
      ? 360.0 + physicsEntity->rotationDelta
      : fmod(adjustedRotation, 360.0);

  }
}

static void handle_entity_collisions() {
  // TODO
}


// TODO - should account for time delta between ticks
void physics_scene_tick() {

  update_entity_positions();

  handle_entity_collisions();
}
