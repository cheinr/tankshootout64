#include "physics.h"

#include <stdint.h>
#include <stdlib.h>
#include <libdragon.h>
#include <math.h>

#define EXPECTED_MAX_PHYSICS_ENTITIES 4
#define USECONDS_PER_SECOND 1000000

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


static int do_entities_collide(struct physics_entity* ours, struct physics_entity* other) {
  return ((ours->position.x - ours->radius <= other->position.x + other->radius)
      && (ours->position.x + ours->radius >= other->position.x - other->radius)
      && (ours->position.y - ours->radius <= other->position.y + other->radius)
      && (ours->position.y + ours->radius >= other->position.y - other->radius));
}

#define PI 3.14159

static void handle_collision(struct physics_entity* ours, struct physics_entity* other) {
  //debugf("handling collision between %lu and %lu!\n", ours->entityId, other->entityId);

  float differenceX = ours->position.x - other->position.x;
  float differenceY = ours->position.y - other->position.y;

  float ourIntersectionX = (differenceX < 0)
    ? (ours->position.x + ours->radius)
    : (ours->position.x - ours->radius);

  float ourIntersectionY = (differenceY < 0)
    ? (ours->position.y + ours->radius)
    : (ours->position.y - ours->radius);

  float otherIntersectionX = (differenceX < 0)
    ? (other->position.x - other->radius)
    : (other->position.x + other->radius);

  float otherIntersectionY = (differenceY < 0)
    ? (other->position.y - other->radius)
    : (other->position.y + other->radius);

  float overlapX = ourIntersectionX - otherIntersectionX;
  float overlapY = ourIntersectionY - otherIntersectionY;

  if (abs(overlapX) < abs(overlapY)) {
    ours->position.x -= overlapX;
  } else {
    ours->position.y -= overlapY;
  }

  double ourRotationRadians = ours->rotation * (PI/180.0);
  float ourDx = cos(ourRotationRadians) * ours->speed;
  float ourDy = sin(ourRotationRadians) * ours->speed;

  other->movementModifier.x = -((ourDx - ours->movementModifier.x) * (1/other->weight));
  other->movementModifier.y = -((ourDy - ours->movementModifier.y) * (1/other->weight));
}

static void update_entity_position(struct physics_entity* physicsEntity, uint32_t timeDeltaUSeconds) {

  float timeDeltaSeconds = timeDeltaUSeconds / (float) USECONDS_PER_SECOND;

  double rotationRadians = physicsEntity->rotation * (PI/180.0);
  float dx = cos(rotationRadians) * physicsEntity->speed * timeDeltaSeconds;
  float dy = sin(rotationRadians) * physicsEntity->speed * timeDeltaSeconds;

  float newX = physicsEntity->position.x + dx - (physicsEntity->movementModifier.x * timeDeltaSeconds);
  float newY = physicsEntity->position.y + dy - (physicsEntity->movementModifier.y * timeDeltaSeconds);

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

  float adjustedRotation = physicsEntity->rotation + (physicsEntity->rotationDelta * timeDeltaSeconds);
  physicsEntity->rotation = (adjustedRotation < 0)
    ? 360.0 + (physicsEntity->rotationDelta * timeDeltaSeconds)
    : fmod(adjustedRotation, 360.0);
}

static void entities_tick(uint32_t timeDeltaUSeconds) {
  for (int i = 0; i < g_physicsScene.physicsEntityCount; i++) {

    struct physics_entity* physicsEntity = g_physicsScene.physicsEntities[i];

    update_entity_position(physicsEntity, timeDeltaUSeconds);

    for (int j = 0; j < g_physicsScene.physicsEntityCount; j++) {
      if (i == j) {
        continue;
      }

      struct physics_entity* other = g_physicsScene.physicsEntities[j];

      if (do_entities_collide(physicsEntity, other)) {
        handle_collision(physicsEntity, other);
      }
    }

    physicsEntity->movementModifier.x = 0;
    physicsEntity->movementModifier.y = 0;
  }
}

void physics_scene_tick(uint32_t timeDeltaUSeconds) {
  entities_tick(timeDeltaUSeconds);
}
