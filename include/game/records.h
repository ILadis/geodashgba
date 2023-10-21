#ifndef RECORDS_H
#define RECORDS_H

#include <gba.h>
#include <game/level.h>

typedef struct Records {
  unsigned int count;
  struct Entry {
    LevelId id;
    // TODO add number of attempts etc.
    int best;
    bool coins[3];
  } entries[200];
} Records;

Records*
Records_GetInstance();

void
Records_LoadFromSave(Records *records);

void
Records_StoreToSave(Records *records);

void
Records_SetBestForLevel(
    Records *records,
    LevelId id, int best);

int
Records_GetBestForLevel(
    Records *records,
    LevelId id);

void
Records_SetCollectedCoinsForLevel(
    Records *records,
    LevelId id, bool *coins);

const bool*
Records_GetCollectedCoinsForLevel(
    Records *records,
    LevelId id);

#endif
