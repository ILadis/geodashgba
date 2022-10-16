#ifndef PARTICLE_H
#define PARTICLE_H

#include <gba.h>
#include <vector.h>
#include <prefab.h>

#include <game/camera.h>
#include <game/move.h>

typedef struct Particle {
  Movement movement;
  int ttl;
  GBA_Sprite *sprite;
} Particle;

void
Particle_ResetAll();

void
Particle_UpdateAll();

void
Particle_DrawAll();

Particle*
Particle_NewInstance(Vector *position);

#endif
