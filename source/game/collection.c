
#include <game/collection.h>

static Collection const* collections[5];

static int GetLevelCount() {
  int Collections_GetLevelCount(Collection const* collections[]);
  return Collections_GetLevelCount(collections);
}

static Binv1Level* GetLevelByIndex(int index) {
  Binv1Level* Collections_GetLevelByIndex(Collection const* collections[], int index);
  return Collections_GetLevelByIndex(collections, index);
}

const Collection*
Collection_GetInstance() {
  static const Collection base = {
    .GetLevelCount = GetLevelCount,
    .GetLevelByIndex = GetLevelByIndex,
  };

  collections[0] = DataCollection_GetInstance();
  collections[1] = DiskCollection_GetInstance();
  collections[2] = NULL;

  return &base;
}

int
Collections_GetLevelCount(Collection const* collections[]) {
  int count = 0;

  const Collection *collection = collections[0];
  for (int i = 0; collection != NULL; collection = collections[++i]) {
    count += Collection_GetLevelCount(collection);
  }

  return count;
}

Binv1Level*
Collections_GetLevelByIndex(
    Collection const* collections[],
    int index)
{
  const Collection *collection = collections[0];
  for (int i = 0; collection != NULL; collection = collections[++i]) {
    int count = Collection_GetLevelCount(collection);
    if (index < count) {
      return Collection_GetLevelByIndex(collection, index);
    }

    index -= count;
  }

  return NULL;
}
