#ifndef RECORDS_H
#define RECORDS_H

#include <gba.h>
#include <game/level.h>

typedef struct Records {
  int count;
  struct Entry {
    LevelId id;
    // TODO add number of attempts, collected stars, etc.
    int best;
  } entries[200];
} Records;

Records*
Records_GetInstance();

void
Records_LoadFromSave(Records *records);

void
Records_StoreToSave(Records *records);

void
Records_AddBestForLevel(
    Records *records,
    LevelId id, int best);

int
Records_GetBestForLevel(
    Records *records,
    LevelId id);

#endif
