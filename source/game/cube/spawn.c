
#include <scene.h>
#include <game/cube.h>
#include <game/particle.h>
#include <game/checkpoint.h>

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

  Checkpoint *checkpoint = Checkpoint_GetInstance();
  const Vector *position = Checkpoint_GetLastPosition(checkpoint);
  if (position != NULL) {
    spawn = position;
  }

  if (Cube_InState(cube, STATE_UNKNOWN)) {
    Cube_SetPosition(cube, spawn);
    Cube_Accelerate(cube, DIRECTION_RIGHT, 160);
  }

  if (Cube_EnteredState(cube, STATE_DESTROYED)) {
    cube->timer = 90;
    if (!cube->success) {
      Course_IncreaseAttempts(course);

      Camera *camera = Camera_GetInstance();
      Camera_Shake(camera);
    }

    const Vector *position = Cube_GetPosition(cube);

    const int delays[] = { 0, 5, 8 };
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
        Course_ResetTo(course, spawn);
      }
    }
  }
}
