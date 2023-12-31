
#include <scene.h>
#include <game/cube.h>
#include <game/particle.h>
#include <game/checkpoint.h>

static inline void
SpawnTrait_ResetCourseToSpawn(
    Course *course,
    const Vector *spawn)
{
  Camera *camera = Camera_GetInstance();

  const int offset = Math_abs(camera->position.x - camera->frame.center.x) - camera->frame.threshold[DIRECTION_LEFT];
  const int x = Math_max(0, spawn->x - offset);

  const Vector position = Vector_Of(x, 0);
  Course_ResetTo(course, &position);
}

static void
SpawnTrait_Apply(
    void *self,
    Course *course)
{
  static const Dynamics dynamics = {
    .friction = { 0, 0 },
    .gravity  = { 0, 0 },
    .limits   = { 5000, 5000 },
  };

  SpawnTrait *trait = self;
  Cube *cube = trait->cube;

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
    trait->timer = 90;
    if (!cube->success) {
      Course_IncreaseAttempts(course);
      SpawnTrait_ResetCourseToSpawn(course, spawn);

      Camera *camera = Camera_GetInstance();
      Camera_Shake(camera);
    }

    const Vector *position = Cube_GetPosition(cube);

    const int delays[] = { 0, 5, 8 };
    for (unsigned int i = 0; i < length(delays); i++) {
      int count = 10;
      while (count-- > 0) {
        Particle *particle = Particle_NewInstance(position, &dynamics, 16, delays[i]);
        Particle_RandomVelocity(particle);
        Particle_RandomSize(particle);
      }
    }
  }
  else if (Cube_InState(cube, STATE_DESTROYED)) {
    int timer = --trait->timer;
    if (timer == 0 && cube->success) {
        extern const Scene *entry;
        Scene *current = Scene_GetCurrent();
        Scene_FadeReplaceWith(current, entry);
    }
    else if (timer <= 0 && Course_AwaitReadyness(course)) {
      Cube_SetPosition(cube, spawn);
      Cube_Accelerate(cube, DIRECTION_RIGHT, 160);
      Cube_SetTraitEnabled(cube, TRAIT_TYPE_MOVE, true);
      Cube_SetTraitEnabled(cube, TRAIT_TYPE_FLY, false);
      Cube_SetTraitEnabled(cube, TRAIT_TYPE_ROTATE, true);
    }
  }
}

Trait*
SpawnTrait_BindTo(
    SpawnTrait *spawn,
    Cube *cube,
    bool enabled)
{
  Trait *trait = &spawn->base;
  trait->self = spawn;
  trait->enabled = enabled;
  trait->type = TRAIT_TYPE_SPAWN;
  trait->Apply = SpawnTrait_Apply;

  spawn->cube = cube;
  spawn->timer = 0;

  Cube_AddTrait(cube, trait);

  return trait;
}
