#ifndef PREFAB_H
#define PREFAB_H

#include <types.h>

typedef bool (*Action)(void *object);
typedef struct Entity Entity;

typedef struct Prefab {
  void *objects;
  int count, size;
  bool locked;
  struct Entity *const entities;
  struct Entity {
    void *object;
    bool alive;
    struct Entity *next;
  } *free, *used;
} Prefab;

#define Prefab_Create(type, limit) &((Prefab) { \
  .objects = (type[limit]){ }, \
  .entities = (Entity[limit]){ }, \
  .count = limit, \
  .size = sizeof(type), \
  .locked = false, \
  .free = NULL, \
  .used = NULL, \
})

void*
Prefab_NewInstance(Prefab *prefabs);

void
Prefab_ForEach(
    Prefab *prefabs,
    Action action);

void
Prefab_ResetAll(Prefab *prefabs);

#endif
