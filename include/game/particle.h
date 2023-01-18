#ifndef PARTICLE_H
#define PARTICLE_H

#include <gba.h>
#include <vector.h>
#include <prefab.h>

#include <game/camera.h>
#include <game/body.h>

typedef struct Particle {
  Body body;
  enum Size {
    PARTICLE_SIZE_SMALL = 6,
    PARTICLE_SIZE_MEDIUM = 5,
    PARTICLE_SIZE_LARGE = 4,
  } size;
  GBA_Sprite *sprite;
  int life, delay;
} Particle;

Particle*
Particle_NewInstance(
    const Vector *position,
    const Dynamics *dynamics,
    int life, int delay);

void
Particle_RandomVelocity(Particle *particle);

static inline void
Particle_SetVelocity(
    Particle *particle,
    int dx, int dy)
{
  Body_SetVelocity(&particle->body, dx, dy);
}

void
Particle_RandomSize(Particle *particle);

static inline void
Particle_SetSize(
    Particle *particle,
    enum Size size)
{
  particle->size = size;
}

void
Particle_ResetAll();

void
Particle_UpdateAll();

void
Particle_DrawAll();

#endif
