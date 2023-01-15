
#include <gba.h>
#include <scene.h>

#include <game/camera.h>
#include <game/cube.h>
#include <game/course.h>
#include <game/level.h>
#include <game/progress.h>
#include <game/particle.h>

static void
Scene_DoEnter() {
  Course *course = Course_GetInstance();

  Progress *progress = Progress_GetInstance();
  Progress_SetCourse(progress, course);

  Camera *camera = Camera_GetInstance();
  Cube *cube = Cube_GetInstance();

  const Vector *position = Cube_GetPosition(cube);
  Camera_FollowTarget(camera, position);

  const Bounds *bounds = Course_GetBounds(course);
  Vector limit = Bounds_Upper(bounds);
  Camera_SetUpperLimit(camera, &limit);

  Camera_Update(camera);
  Particle_ResetAll();

  mGBA_DebugLog(mGBA_LOG_INFO, "Scene: play");
}

static void
Scene_DoPlay() {
  static bool debug = false;
  GBA_Input *input = GBA_GetInput();

  GBA_Input_PollStates(input);

  Cube *cube = Cube_GetInstance();
  Camera *camera = Camera_GetInstance();
  Course *course = Course_GetInstance();
  Progress *progress = Progress_GetInstance();

  if (GBA_Input_IsPressed(input, GBA_KEY_A)) {
    Cube_Jump(cube, 1400);
  }

  if (GBA_Input_IsHit(input, GBA_KEY_SELECT)) {
    debug = true;
  }

  if (debug) {
    if (GBA_Input_IsHeld(input, GBA_KEY_LEFT)) {
      Cube_Accelerate(cube, DIRECTION_LEFT, 160);
    } else if (GBA_Input_IsHeld(input, GBA_KEY_RIGHT)) {
      Cube_Accelerate(cube, DIRECTION_RIGHT, 160);
    } else {
      Cube_Accelerate(cube, DIRECTION_RIGHT, 0);
    }
  }

  Cube_Update(cube, course);
  Camera_Update(camera);
  Progress_Update(progress, cube);
  Particle_UpdateAll();

  GBA_VSync();

  if (debug && Cube_InState(cube, STATE_DESTROYED)) {
    extern void Debug_DrawHitboxes();
    Debug_DrawHitboxes();
    while (debug);
  }

  Cube_Draw(cube, camera);
  Course_Draw(course, camera);
  Progress_Draw(progress);
  Particle_DrawAll();

  if (GBA_Input_IsHit(input, GBA_KEY_B)) {
    extern const Scene *entry;
    Scene *current = Scene_GetCurrent();
    Scene_FadeReplaceWith(current, entry);
  }
}

static void
Scene_DoExit() {
  Camera *camera = Camera_GetInstance();
  Camera_Reset(camera);

  Cube *cube = Cube_GetInstance();
  Cube_Reset(cube);

  Course *course = Course_GetInstance();
  Course_ResetAndLoad(course, NULL);

  Particle_ResetAll();
}

const Scene *play = &(Scene) {
  .enter = Scene_DoEnter,
  .play = Scene_DoPlay,
  .exit = Scene_DoExit,
};
