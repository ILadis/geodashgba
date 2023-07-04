#ifndef COLLECTION_H
#define COLLECTION_H

#include <types.h>
#include <game/level.h>

typedef struct Collection {
  char signature[16];
  int length;
  short count;
  int allocations[20];
} packed Collection;

#define Collection_DefineWithUsableSpace(size) Collection_DefineNew(sizeof(Collection) + size)
#define Collection_DefineNew(size) (Collection *) \
((align8 unsigned char[size]) { \
  'L', 'e', 'v', 'e', 'l', 'C', 'o', 'l', 'l', 'e', 'c', 't', 'i', 'o', 'n', '!', \
  (unsigned char) ((size >>  0) & 0xFF), \
  (unsigned char) ((size >>  8) & 0xFF), \
  (unsigned char) ((size >> 16) & 0xFF), \
  (unsigned char) ((size >> 24) & 0xFF), \
})

int
Collection_GetLevelCount(const Collection *collection);

Level*
Collection_GetLevelByIndex(
    const Collection *collection,
    int index);

bool
Collection_AddLevel(
    Collection *collection,
    const Level *level);

// TODO consider Collection_AddLevel and Collection_RemoveLevel

#endif
