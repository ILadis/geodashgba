#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <gba.h>
#include <vector.h>

#include <game/camera.h>

typedef struct Checkpoint {
  GBA_Sprite *sprites[4];
  Vector positions[4], *last;
  int index, count;
} Checkpoint;

Checkpoint*
Checkpoint_GetInstance();

void
Checkpoint_Reset(Checkpoint *checkpoint);

const Vector*
Checkpoint_GetLastPosition(Checkpoint *checkpoint);

bool
Checkpoint_RemoveLastPosition(Checkpoint *checkpoint);

void
Checkpoint_AddPosition(
    Checkpoint *checkpoint,
    const Vector *position);

void
Checkpoint_Draw(
    Checkpoint *checkpoint,
    Camera *camera);

#endif
