
#include <game/records.h>

Records*
Records_GetInstance() {
  static Records records = {0};
  return &records;
}

void
Records_LoadFromSave(unused Records *records) {
  // TODO implement this
}

void
Records_StoreToSave(unused Records *records) {
  // TODO implement this
}

static inline struct Entry*
Records_FindEntryForLevel(
    Records *records,
    LevelId id)
{
  int count = records->count;
  for (int index = 0; index < count; index++) {
    struct Entry *entry = &records->entries[index];

    if (entry->id == id) {
      return entry;
    }
  }

  return NULL;
}

static inline struct Entry*
Records_AddNewEntryForLevel(
    Records *records,
    LevelId id)
{
  unsigned int index = records->count;
  if (index + 1 >= length(records->entries)) {
    return NULL;
  }

  struct Entry *entry = &records->entries[index];
  entry->id = id;
  records->count++;

  return entry;
}

void
Records_SetBestForLevel(
    Records *records,
    LevelId id, int best)
{
  struct Entry *entry = Records_FindEntryForLevel(records, id);
  if (entry == NULL) {
    entry = Records_AddNewEntryForLevel(records, id);
  }

  if (entry != NULL && best > entry->best) {
    entry->best = best;
  }
}

int
Records_GetBestForLevel(
    Records *records,
    LevelId id)
{
  struct Entry *entry = Records_FindEntryForLevel(records, id);
  if (entry == NULL) {
    return 0;
  }

  return entry->best;
}

void
Records_SetCollectedCoinsForLevel(
    Records *records,
    LevelId id, bool *coins)
{
  struct Entry *entry = Records_FindEntryForLevel(records, id);
  if (entry == NULL) {
    entry = Records_AddNewEntryForLevel(records, id);
  }

  if (entry != NULL) {
    entry->coins[0] = coins[0];
    entry->coins[1] = coins[1];
    entry->coins[2] = coins[2];
  }
}

const bool*
Records_GetCollectedCoinsForLevel(
    Records *records,
    LevelId id)
{
  static const bool fallback[3] = { false };

  struct Entry *entry = Records_FindEntryForLevel(records, id);
  if (entry == NULL) {
    return fallback;
  }

  return entry->coins;
}
