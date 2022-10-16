
#include <gba.h>
#include <text.h>
#include <scene.h>

#include <assets/graphics/tiles.h>
#include <assets/graphics/sprites.h>

#include <game/camera.h>
#include <game/cube.h>
#include <game/loader.h>
#include <game/course.h>
#include <game/object.h>

static void
Scene_DoEnter() {
  GBA_Sprite_ResetAll();
  GBA_EnableSprites();

  GBA_System *system = GBA_GetSystem();

  // used for tilemaps
  GBA_Memcpy(&system->tileSets8[0][0], tilesTiles, tilesTilesLen);
  GBA_Memcpy(&system->backgroundPalette[0], tilesPal, tilesPalLen);

  // used for sprites
  GBA_Memcpy(&system->tileSets4[4][0], spritesTiles, spritesTilesLen);
  GBA_Memcpy(&system->spritePalette[0], spritesPal, spritesPalLen);

  Cube *cube = Cube_GetInstance();
  Cube_Reset(cube);
  Cube_Accelerate(cube, DIRECTION_RIGHT, 160);

  Camera *camera = Camera_GetInstance();
  Camera_Reset(camera);

  Course *course = Course_GetInstance();
  Course_Reset(course);

  Vector *position = Cube_GetPosition(cube);
  Camera_FollowTarget(camera, position);
  Camera_Update(camera);

  Loader *loader = Loader_ForTestCourse();
  Loader_LoadCourse(loader, course);
}

static void
Scene_DoPlay() {
  static bool debug = false;
  GBA_Input *input = GBA_GetInput();

  GBA_Input_PollStates(input);

  Cube *cube = Cube_GetInstance();
  Camera *camera = Camera_GetInstance();
  Course *course = Course_GetInstance();

  if (GBA_Input_IsPressed(input, GBA_KEY_A)) {
    Cube_Jump(cube);
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

  Cube_Update(cube);
  Bounds *hitbox = Cube_GetHitbox(cube);
  Hit hit = Course_CheckHits(course, hitbox);
  Cube_TakeHit(cube, &hit);
  Cube_HandleRotation(cube, course);

  Camera_Update(camera);
  GBA_VSync();

  Cube_Draw(cube, camera);
  Course_Draw(course, camera);

/*
  if (GBA_Input_IsHit(input, GBA_KEY_A)) {
      extern const Scene *game;
      Scene *current = Scene_GetCurrent();
      Scene_ReplaceWith(current, game);
  }
*/
}

const Scene *entry = &(Scene) {
  .enter = Scene_DoEnter,
  .play = Scene_DoPlay,
  .exit = Scene_Noop,
};
