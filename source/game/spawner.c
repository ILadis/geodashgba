
#include <game/spawner.h>
#include <game/course.h>

Spawner*
Spawner_GetInstance() {
  static Spawner spawner = {0};
  return &spawner;
}

void
Spawner_Update(Spawner *spawner) {
  Cube *cube = spawner->cube;

  if (Cube_InState(cube, STATE_UNKNOWN)) {
    Cube_SetPosition(cube, spawner->position);
    Cube_Accelerate(cube, DIRECTION_RIGHT, 160);
  }

  if (Cube_EnteredState(cube, STATE_DESTROYED)) {
    spawner->attempts++;
    spawner->timer = 90;

    Vector *position = Cube_GetPosition(cube);
    int count;

    count = 10;
    while (count-- > 0) Particle_NewInstance(position, 16, 0);

    count = 10;
    while (count-- > 0) Particle_NewInstance(position, 16, 5);

    count = 10;
    while (count-- > 0) Particle_NewInstance(position, 16, 8);
  }
  else if (Cube_InState(cube, STATE_DESTROYED)) {
    int timer = spawner->timer -= 1;
    if (timer == 0) {
      Cube_SetPosition(cube, spawner->position);
      Cube_Accelerate(cube, DIRECTION_RIGHT, 160);

      // TODO workaround to force redraw after camera resets to spawn
      Course *course = Course_GetInstance();
      Course_ResetAndLoad(course, NULL);
    }
  }
}

void
Spawner_Draw(
    Spawner *spawner,
    Camera *camera)
{

}
