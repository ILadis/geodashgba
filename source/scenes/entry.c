
#include <gba.h>
#include <text.h>
#include <scene.h>

#include <assets/graphics/tiles.h>
#include <assets/graphics/sprites.h>

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
    .priority = 3,
  };

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 0);

  extern const GBA_TileMapRef backgroundTileMap;
  GBA_TileMapRef_Blit(&target, 0, 0, &backgroundTileMap);
}

static void
Scene_DoPlay() {
  GBA_System *system = GBA_GetSystem();
  GBA_Input *input = &system->input;

  GBA_Input_PollStates(input);

  GBA_VSync();

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
