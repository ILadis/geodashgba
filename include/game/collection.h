#ifndef COLLECTION_H
#define COLLECTION_H

#include <types.h>
#include <io.h>
#include <game/level.h>

typedef struct Collection {
  char signature[16];
  int length;
  short count;
  int allocations[20];
} packed Collection;

#define Collection_DefineWithUsableSpace(size) Collection_DefineNew(, sizeof(Collection) + size)
#define Collection_DefineWithTotalSpace(size) Collection_DefineNew(, size)

#define Collection_DefineNew(modifier, size) (Collection *) \
((align8 modifier unsigned char[size]) { \
  'L', 'e', 'v', 'e', 'l', 'C', 'o', 'l', 'l', 'e', 'c', 't', 'i', 'o', 'n', '!', \
  (unsigned char) (((size) >>  0) & 0xFF), \
  (unsigned char) (((size) >>  8) & 0xFF), \
  (unsigned char) (((size) >> 16) & 0xFF), \
  (unsigned char) (((size) >> 24) & 0xFF), \
})

const Collection*
Collection_GetInstance();

int
Collection_GetLevelCount(const Collection *collection);

Binv1Level*
Collection_GetLevelByIndex(
    const Collection *collection,
    int index);

bool
Collection_AddLevel(
    Collection *collection,
    const Binv1Level *level);

// TODO implement Collection_RemoveLevel

bool
Collection_FindSignature(const Reader *reader);

bool
Collection_ReadFrom(
    volatile Collection *collection,
    const Reader *reader);

bool
Collection_WriteTo(
    const Collection *collection,
    const Writer *writer);

#endif
