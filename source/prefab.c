
#include <prefab.h>

void*
Prefab_NewInstance(Prefab *prefabs) {
  Entity *free = prefabs->free;
  Entity *used = prefabs->used;

  if (free == NULL) {
    return NULL;
  }

  Entity *next = free->next;

  void *object = free->object;

  free->alive = true;
  free->next = used;

  prefabs->used = free;
  prefabs->free = next;

  return object;
}

static inline Entity*
Prefab_FreeInstance(
    Prefab *prefabs,
    Entity *entity,
    Entity *before)
{
  Entity *next = entity->next;

  if (before == NULL) {
    prefabs->used = next;
  } else {
    before->next = next;
  }

  entity->next = prefabs->free;
  prefabs->free = entity;

  return next;
}

static inline bool
Prefab_SetLock(Prefab *prefabs) {
  bool locked = prefabs->locked;
  bool exclusive = !locked;

  prefabs->locked = true;

  return exclusive;
}

void
Prefab_ForEach(
    Prefab *prefabs,
    Action action)
{
  bool exclusive = Prefab_SetLock(prefabs);

  Entity *entity = prefabs->used;
  Entity *before = NULL;

  while (entity != NULL) {
    bool alive = entity->alive ? action(entity->object) : false;

    if (!alive && exclusive) {
      entity = Prefab_FreeInstance(prefabs, entity, before);
      continue;
    }

    entity->alive = alive;

    before = entity;
    entity = entity->next;
  }

  prefabs->locked = !exclusive;
}

void
Prefab_ResetAll(Prefab *prefabs) {
  void *objects = prefabs->objects;
  int count = prefabs->count;
  int size = prefabs->size;

  Entity *used = prefabs->used;
  while (used != NULL) {
    used = Prefab_FreeInstance(prefabs, used, NULL);
  }

  Entity *entity = prefabs->free;

  for (int i = 0; i < count; i++) {
    Entity *next = entity + 1;
    void *object = objects + i * size ;

    entity->alive = false;
    entity->next = next;
    entity->object = object;

    entity = next;
  }

  (entity - 1)->next = NULL;
}
