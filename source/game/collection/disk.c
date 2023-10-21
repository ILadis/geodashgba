
#include <game/collection.h>

static DiskCollection collection = {0};

static int GetLevelCount() {
  int DiskCollection_GetLevelCount(DiskCollection *collection);
  return DiskCollection_GetLevelCount(&collection);
}

static Binv1Level* GetLevelByIndex(int index) {
  Binv1Level* DiskCollection_GetLevelByIndex(DiskCollection *collection, int index);
  return DiskCollection_GetLevelByIndex(&collection, index);
}

const Collection*
DiskCollection_GetInstance() {
  static const Collection base = {
    .GetLevelCount = GetLevelCount,
    .GetLevelByIndex = GetLevelByIndex,
  };

  return &base;
}

static bool
DiskCollection_InitializeDisk(DiskCollection *collection) {
  if (!Everdrive_UnlockSystem()) {
    return false;
  }

  Disk *disk = &collection->disk;
  if (!Everdrive_CardInitialize()) {
    return false;
  }

  if (!Disk_Initialize(disk, Everdrive_CardReadBlock)) {
    return false;
  }

  return true;
}

static inline bool
DiskCollection_IsLevelEntry(DiskEntry *entry) {
  const char extension[] = "BIN";

  const int length = length(extension);
  const int offset = length(entry->name) - length;

  char *name = &entry->name[offset];

  for (int i = 0; i < length; i++) {
    if (name[i] != extension[i]) {
      return false;
    }
  }

  return true;
}

static bool
DiskCollection_ScanForLevels(
    DiskCollection *collection,
    char **path)
{
  Disk *disk = &collection->disk;
  if (!Disk_OpenDirectory(disk, path)) {
    return false;
  }

  const int capacity = length(collection->levels);
  int index = collection->count;

  DiskEntry entry = {0};
  while (Disk_ReadDirectory(disk, &entry)) {
    if (DiskCollection_IsLevelEntry(&entry)) {
      collection->levels[index++] = entry;
      collection->count++;

      if (index >= capacity) break;
    }
  }

  return true;
}

int
DiskCollection_GetLevelCount(DiskCollection *collection) {
  static bool once = true;

  if (once) {
    char *directory[] = { NULL };

    if (DiskCollection_InitializeDisk(collection)) {
      DiskCollection_ScanForLevels(collection, directory);
    }

    once = false;
  }

  return collection->count;
}

Binv1Level*
DiskCollection_GetLevelByIndex(
    DiskCollection *collection,
    int index)
{
  static Binv1Level level = {0};

  if (index < collection->count && index >= 0) {
    Disk *disk = &collection->disk;
    DiskEntry *entry = &collection->levels[index];

    DataSource *source = Disk_OpenFile(disk, entry);
    if (source == NULL) {
      return NULL;
    }

    Binv1Level_From(&level, source);
    return &level;
  }

  return NULL;
}
