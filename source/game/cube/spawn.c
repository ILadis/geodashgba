
#include <scene.h>
#include <game/cube.h>
#include <game/particle.h>

void
Cube_ApplySpawn(
    Cube *cube,
    Course *course)
{
  const Vector *spawn = Course_GetSpawn(course);

  if (Cube_InState(cube, STATE_UNKNOWN)) {
    Cube_SetPosition(cube, spawn);
    Cube_Accelerate(cube, DIRECTION_RIGHT, 160);
  }

  if (Cube_EnteredState(cube, STATE_DESTROYED)) {
    cube->attempts++;
    cube->timer = 90;

    const Vector *position = Cube_GetPosition(cube);
    int count;

    count = 10;
    while (count-- > 0) Particle_NewInstance(position, 16, 0);

    count = 10;
    while (count-- > 0) Particle_NewInstance(position, 16, 5);

    count = 10;
    while (count-- > 0) Particle_NewInstance(position, 16, 8);
  }
  else if (Cube_InState(cube, STATE_DESTROYED)) {
    int timer = cube->timer -= 1;
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
