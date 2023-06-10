
#include <gba.h>
#include <log.h>
#include <scene.h>

#include <game/camera.h>
#include <game/cube.h>
#include <game/checkpoint.h>
#include <game/course.h>
#include <game/level.h>
#include <game/progress.h>
#include <game/selector.h>
#include <game/records.h>
#include <game/particle.h>

extern Logger* mGBA_GetLogger();

static void
Scene_DoEnter() {
  Course *course = Course_GetInstance();

  Progress *progress = Progress_GetInstance();
  Progress_SetMode(progress, MODE_PLAY);
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

  Logger *logger = mGBA_GetLogger();
  Logger_PrintLine(logger, "Scene: play");
}

static void
Scene_DoPlay() {
  static bool debug = false;

  GBA_Input_PollStates();

  Cube *cube = Cube_GetInstance();
  Checkpoint *checkpoint = Checkpoint_GetInstance();
  Camera *camera = Camera_GetInstance();
  Course *course = Course_GetInstance();
  Progress *progress = Progress_GetInstance();

  if (GBA_Input_IsPressed(GBA_KEY_A)) {
    Cube_Jump(cube, 1400);
  }

  if (GBA_Input_IsHit(GBA_KEY_R)) {
    Checkpoint_AddPosition(checkpoint, Cube_GetPosition(cube));
  } else if (GBA_Input_IsHit(GBA_KEY_L)) {
    Checkpoint_RemoveLastPosition(checkpoint);
  }

  if (GBA_Input_IsHit(GBA_KEY_SELECT)) {
    debug = true;
  }

  if (debug) {
    if (GBA_Input_IsHeld(GBA_KEY_LEFT)) {
      Cube_Accelerate(cube, DIRECTION_LEFT, 160);
    } else if (GBA_Input_IsHeld(GBA_KEY_RIGHT)) {
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

  Course_Draw(course, camera);
  Cube_Draw(cube, camera);
  Checkpoint_Draw(checkpoint, camera);
  Progress_Draw(progress);
  Particle_DrawAll();

  if (GBA_Input_IsHit(GBA_KEY_B)) {
    extern const Scene *entry;
    Scene *current = Scene_GetCurrent();
    Scene_FadeReplaceWith(current, entry);
  }
}

static void
Scene_DoExit() {
  Camera *camera = Camera_GetInstance();
  Camera_Reset(camera);

  Checkpoint *checkpoint = Checkpoint_GetInstance();
  Checkpoint_Reset(checkpoint);

  Progress *progress = Progress_GetInstance();
  int best = Progress_GetBestProgress(progress);
  bool *coins = Progress_GetCollectedCoins(progress);

  Selector *selector = Selector_GetInstance();
  LevelId id = Selector_GetLevelId(selector);

  Records *records = Records_GetInstance();
  Records_SetBestForLevel(records, id, best);

  Cube *cube = Cube_GetInstance();
  if (cube->success) {
    Records_SetCollectedCoinsForLevel(records, id, coins);
  }

  Cube_Reset(cube);
  Particle_ResetAll();
}

const Scene *play = &(Scene) {
  .enter = Scene_DoEnter,
  .play = Scene_DoPlay,
  .exit = Scene_DoExit,
};
