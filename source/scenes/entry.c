
#include <gba.h>
#include <text.h>
#include <scene.h>

#include <assets/graphics/tiles.h>
#include <assets/graphics/sprites.h>

#include <game/camera.h>
#include <game/cube.h>
#include <game/course.h>

static void
Scene_DoEnter() {
  GBA_Sprite_ResetAll();
  GBA_System *system = GBA_GetSystem();

  // used for tilemaps
  GBA_Memcpy(&system->tileSets8[0][0], tilesTiles, tilesTilesLen);
  GBA_Memcpy(&system->backgroundPalette[0], tilesPal, tilesPalLen);

  // used for sprites
  GBA_Memcpy(&system->tileSets4[4][0], spritesTiles, spritesTilesLen);
  GBA_Memcpy(&system->spritePalette[0], spritesPal, spritesPalLen);

  // setup backgrounds
  system->displayControl[0] = (GBA_DisplayControl) {
    .mode = 0,
    .enableBG0 = 1,
    .enableOBJ = 1,
    .sprite1DMapping = 1,
  };

  system->backgroundControls[0] = (GBA_BackgroundControl) {
    .colorMode = 1,
    .tileSetIndex = 0,
    .tileMapIndex = 8,
    .priority = 2,
  };

  GBA_Sprite_ResetAll();

  Cube *cube = Cube_GetInstance();
  Cube_Reset(cube);

  Camera *camera = Camera_GetInstance();
  Camera_Reset(camera);

  Course *course = Course_GetInstance();
  Course_Reset(course);
}

static void
Scene_DoPlay() {
  GBA_System *system = GBA_GetSystem();
  GBA_Input *input = &system->input;

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
