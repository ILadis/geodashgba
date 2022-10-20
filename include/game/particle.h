#ifndef PARTICLE_H
#define PARTICLE_H

#include <gba.h>
#include <vector.h>
#include <prefab.h>

#include <game/camera.h>
#include <game/body.h>

typedef struct Particle {
  Body body;
  GBA_Sprite *sprite;
  int life, delay;
} Particle;

void
Particle_ResetAll();

void
Particle_UpdateAll();

void
Particle_DrawAll();

Particle*
Particle_NewInstance(
    Vector *position,
    int life, int delay);

#endif
