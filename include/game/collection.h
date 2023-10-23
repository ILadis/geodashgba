#ifndef COLLECTION_H
#define COLLECTION_H

#include <types.h>
#include <everdrive.h>
#include <disk.h>
#include <io.h>
#include <game/level.h>

typedef struct Collection {
  int (*GetLevelCount)();
  Binv1Level* (*GetLevelByIndex)(int index);
} Collection;

const Collection*
Collection_GetInstance();

static inline int
Collection_GetLevelCount(const Collection *collection) {
  return collection->GetLevelCount();
}

static inline Binv1Level*
Collection_GetLevelByIndex(const Collection *collection, int index) {
  return collection->GetLevelByIndex(index);
}

typedef struct DiskCollection {
  Disk disk;
  DiskEntry levels[15];
  int count;
} DiskCollection;

const Collection*
DiskCollection_GetInstance();

typedef struct DataCollection {
  char signature[16];
  unsigned int length;
  volatile unsigned short count;
  volatile unsigned int allocations[20];
} packed align4 DataCollection;

const Collection*
DataCollection_GetInstance();

#define DataCollection_DefineWithUsableSpace(size) DataCollection_DefineNew(, sizeof(DataCollection) + size)
#define DataCollection_DefineWithTotalSpace(size) DataCollection_DefineNew(, size)

#define DataCollection_DefineNew(modifier, size) (DataCollection *) \
((modifier unsigned char[size]) { \
  'L', 'e', 'v', 'e', 'l', 'C', 'o', 'l', 'l', 'e', 'c', 't', 'i', 'o', 'n', '!', \
  (unsigned char) (((size) >>  0) & 0xFF), \
  (unsigned char) (((size) >>  8) & 0xFF), \
  (unsigned char) (((size) >> 16) & 0xFF), \
  (unsigned char) (((size) >> 24) & 0xFF), \
})

int
DataCollection_GetLevelCount(const DataCollection *collection);

Binv1Level*
DataCollection_GetLevelByIndex(
    const DataCollection *collection,
    int index);

bool
DataCollection_AddLevel(
    DataCollection *collection,
    const Binv1Level *level);

// TODO implement DataCollection_RemoveLevel

bool
DataCollection_FindSignature(const Reader *reader);

bool
DataCollection_ReadFrom(
    DataCollection *collection,
    const Reader *reader);

bool
DataCollection_WriteTo(
    const DataCollection *collection,
    const Writer *writer);

#endif
