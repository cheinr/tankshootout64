#include "id.h"

uint32_t entityIdCounter = 0;

uint32_t next_entity_id() {
  return entityIdCounter++;
}
