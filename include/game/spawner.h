#ifndef SPAWNER_H
#define SPAWNER_H

#include <gba.h>
#include <vector.h>

#include <game/cube.h>
#include <game/camera.h>
#include <game/particle.h>

typedef struct Spawner {
  Cube *cube;
  const Vector *position;
  int attempts, timer;
} Spawner;

Spawner*
Spawner_GetInstance();

static inline void
Spawner_SetTarget(
    Spawner *spawner,
    Cube *cube,
    const Vector *position)
{
  spawner->cube = cube;
  spawner->position = position;
}

void
Spawner_Update(Spawner *spawner);

void
Spawner_Draw(
    Spawner *spawner,
    Camera *camera);

#endif
