
#include <gba.h>
#include <text.h>
#include <scene.h>

#include <assets/graphics/tiles.h>
#include <assets/graphics/sprites.h>

#include <game/camera.h>
#include <game/cube.h>
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

  Camera *camera = Camera_GetInstance();
  Camera_Reset(camera);

  Course *course = Course_GetInstance();
  Course_Reset(course);

  Vector *position = Cube_GetPosition(cube);
  Camera_FollowTarget(camera, position);

  Object *object = NULL;

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(14, 12));

  object = Course_AddObject(course);
  Object_CreateBox(object);
  Object_SetPosition(object, &Vector_Of(20, 15));

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(20, 10));

  object = Course_AddObject(course);
  Object_CreateBox(object);
  Object_SetPosition(object, &Vector_Of(26, 15));

  object = Course_AddObject(course);
  Object_CreateBox(object);
  Object_SetPosition(object, &Vector_Of(26, 13));

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(26, 8));

  object = Course_AddObject(course);
  Object_CreateBox(object);
  Object_SetPosition(object, &Vector_Of(29, 15));

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(29, 10));

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(33, 6));

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(37, 2));

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(41, -2));

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(45, -6));

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(49, -10));

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(53, -14));

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(57, -18));

  object = Course_AddObject(course);
  Object_CreateBlockWithPole(object);
  Object_SetPosition(object, &Vector_Of(38, 12));
}

static void
Scene_DoPlay() {
  GBA_Input *input = GBA_GetInput();

  GBA_Input_PollStates(input);

  Cube *cube = Cube_GetInstance();
  Camera *camera = Camera_GetInstance();
  Course *course = Course_GetInstance();

  Cube_Update(cube);
  Hit hit = Course_CheckHits(course, cube);

  Cube_TakeHit(cube, &hit);

  if (GBA_Input_IsHit(input, GBA_KEY_A)) {
    Cube_Jump(cube);
  }

  if (GBA_Input_IsHeld(input, GBA_KEY_LEFT)) {
    Cube_Accelerate(cube, DIRECTION_LEFT);
  } else if (GBA_Input_IsHeld(input, GBA_KEY_RIGHT)) {
    Cube_Accelerate(cube, DIRECTION_RIGHT);
  }

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
