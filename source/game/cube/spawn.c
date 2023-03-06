
#include <scene.h>
#include <game/cube.h>
#include <game/particle.h>

void
Cube_ApplySpawn(
    Cube *cube,
    Course *course)
{
  static const Dynamics dynamics = {
    .friction = { 0, 0 },
    .gravity  = { 0, 0 },
    .limits   = { 5000, 5000 },
  };

  const Vector *spawn = Course_GetSpawn(course);

  if (Cube_InState(cube, STATE_UNKNOWN)) {
    Cube_SetPosition(cube, spawn);
    Cube_Accelerate(cube, DIRECTION_RIGHT, 160);
  }

  if (Cube_EnteredState(cube, STATE_DESTROYED)) {
    cube->attempts++;
    cube->timer = 90;

    const Vector *position = Cube_GetPosition(cube);

    const int delays[] = {0, 5, 8};
    for (int i = 0; i < length(delays); i++) {
      int count = 10;
      while (count-- > 0) {
        Particle *particle = Particle_NewInstance(position, &dynamics, 16, delays[i]);
        Particle_RandomVelocity(particle);
        Particle_RandomSize(particle);
      }
    }
  }
  else if (Cube_InState(cube, STATE_DESTROYED)) {
    int timer = --cube->timer;
    if (timer == 0) {
      if (cube->success) {
        extern const Scene *entry;
        Scene *current = Scene_GetCurrent();
        Scene_FadeReplaceWith(current, entry);
      }
      else {
        Cube_SetPosition(cube, spawn);
        Cube_Accelerate(cube, DIRECTION_RIGHT, 160);
        // TODO workaround to force redraw after camera resets to spawn
        Course_ResetAndLoad(course, NULL);
      }
    }
  }
}
