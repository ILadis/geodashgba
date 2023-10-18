
#include <gba.h>
#include "test.h"

test(Sprite_ShapeOf_ShouldReturnExpectedValues) {
  assert(GBA_SPRITE_SHAPE_OF( 8,  8) == 0);
  assert(GBA_SPRITE_SHAPE_OF(16, 16) == 0);
  assert(GBA_SPRITE_SHAPE_OF(32, 32) == 0);
  assert(GBA_SPRITE_SHAPE_OF(64, 64) == 0);

  assert(GBA_SPRITE_SHAPE_OF(16,  8) == 1);
  assert(GBA_SPRITE_SHAPE_OF(32,  8) == 1);
  assert(GBA_SPRITE_SHAPE_OF(32, 16) == 1);
  assert(GBA_SPRITE_SHAPE_OF(64, 32) == 1);

  assert(GBA_SPRITE_SHAPE_OF( 8, 16) == 2);
  assert(GBA_SPRITE_SHAPE_OF( 8, 32) == 2);
  assert(GBA_SPRITE_SHAPE_OF(16, 32) == 2);
  assert(GBA_SPRITE_SHAPE_OF(32, 64) == 2);
}

test(Sprite_SizeOf_ShouldReturnExpectedValues) {
  assert(GBA_SPRITE_SIZE_OF( 8,  8) == 0); // >= 64px²
  assert(GBA_SPRITE_SIZE_OF(16,  8) == 0);
  assert(GBA_SPRITE_SIZE_OF( 8, 16) == 0); // <= 128px²

  assert(GBA_SPRITE_SIZE_OF(16, 16) == 1); // <= 256px²
  assert(GBA_SPRITE_SIZE_OF(32,  8) == 1);
  assert(GBA_SPRITE_SIZE_OF( 8, 32) == 1); // <= 256px²

  assert(GBA_SPRITE_SIZE_OF(32, 32) == 2); // >= 512px²
  assert(GBA_SPRITE_SIZE_OF(32, 16) == 2);
  assert(GBA_SPRITE_SIZE_OF(16, 32) == 2); // <= 1024px²

  assert(GBA_SPRITE_SIZE_OF(64, 64) == 3); // >= 248px²
  assert(GBA_SPRITE_SIZE_OF(64, 32) == 3);
  assert(GBA_SPRITE_SIZE_OF(32, 64) == 3); // <= 4096px²
}

test(Sprite_Allocate_ShouldReturnNewSpriteInstanceForEachCall) {
  // arrange
  GBA_Sprite_ResetAll();

  // act
  GBA_Sprite *sprite1 = GBA_Sprite_Allocate();
  GBA_Sprite *sprite2 = GBA_Sprite_Allocate();
  GBA_Sprite *sprite3 = GBA_Sprite_Allocate();

  // assert
  assert(sprite1 != NULL);
  assert(sprite2 != NULL);
  assert(sprite3 != NULL);

  assert(sprite1 != sprite2);
  assert(sprite2 != sprite3);
  assert(sprite3 != sprite1);
}

test(Sprite_Release_ShouldMakeReleasedSpriteAvailableAgain) {
  // arrange
  GBA_Sprite_ResetAll();

  GBA_Sprite *sprite1 = GBA_Sprite_Allocate();
  GBA_Sprite *sprite2 = GBA_Sprite_Allocate();
  GBA_Sprite *sprite3 = GBA_Sprite_Allocate();

  // act
  GBA_Sprite_Release(sprite2);
  GBA_Sprite *sprite4 = GBA_Sprite_Allocate();

  // assert
  assert(sprite2 == sprite4);

  assert(sprite2 != sprite1);
  assert(sprite2 != sprite3);
}

test(Sprite_ResetAll_ShouldMakeAllUsedSpritesAvailableAgain) {
  // arrange
  GBA_Sprite_ResetAll();

  GBA_Sprite *sprite1 = GBA_Sprite_Allocate();
  GBA_Sprite *sprite2 = GBA_Sprite_Allocate();

  // act
  GBA_Sprite_ResetAll();

  GBA_Sprite *sprite3 = GBA_Sprite_Allocate();
  GBA_Sprite *sprite4 = GBA_Sprite_Allocate();

  // assert
  assert(sprite1 != NULL);
  assert(sprite2 != NULL);

  assert(sprite1 == sprite3);
  assert(sprite2 == sprite4);
}

test(Sprite_GetAffine_ShouldReturnNullWhenSpriteHasNoAffineAssigned) {
  // arrange
  GBA_Sprite_ResetAll();

  // act
  GBA_Sprite *sprite = GBA_Sprite_Allocate();
  GBA_Affine *affine = GBA_Sprite_GetAffine(sprite);

  // assert
  assert(sprite != NULL);
  assert(affine == NULL);
}

test(Sprite_GetAffine_ShouldReturnPreviouslyAssignedAffine) {
  // arrange
  GBA_Sprite_ResetAll();

  GBA_Sprite *sprite1 = GBA_Sprite_Allocate();
  GBA_Sprite *sprite2 = GBA_Sprite_Allocate();

  GBA_Affine *affine1 = GBA_Affine_Allocate();

  GBA_Sprite_SetAffine(sprite1, affine1, false);
  GBA_Sprite_SetAffine(sprite2, affine1, false);

  // act
  GBA_Affine *affine2 = GBA_Sprite_GetAffine(sprite1);
  GBA_Affine *affine3 = GBA_Sprite_GetAffine(sprite2);

  // assert
  assert(affine1 != NULL);

  assert(affine1 == affine2);
  assert(affine2 == affine3);
  assert(affine3 == affine1);
}

test(Affine_Allocate_ShouldReturnNewAffineInstanceForEachCall) {
  // arrange
  GBA_Sprite_ResetAll();

  // act
  GBA_Affine *affine1 = GBA_Affine_Allocate();
  GBA_Affine *affine2 = GBA_Affine_Allocate();
  GBA_Affine *affine3 = GBA_Affine_Allocate();

  // assert
  assert(affine1 != NULL);
  assert(affine2 != NULL);
  assert(affine3 != NULL);

  assert(affine1 != affine2);
  assert(affine2 != affine3);
  assert(affine3 != affine1);
}

test(Affine_Release_ShouldMakeReleasedAffineAvailableAgain) {
  // arrange
  GBA_Sprite_ResetAll();

  GBA_Affine *affine1 = GBA_Affine_Allocate();
  GBA_Affine *affine2 = GBA_Affine_Allocate();
  GBA_Affine *affine3 = GBA_Affine_Allocate();

  // act
  GBA_Affine_Release(affine2);
  GBA_Affine *affine4 = GBA_Affine_Allocate();

  // assert
  assert(affine2 == affine4);

  assert(affine2 != affine1);
  assert(affine2 != affine3);
}

test(Input_IsHit_ShoudReturnTrueWhenQueriedKeyWasHitInLastKeypadPoll) {
  // arrange
  GBA_System *system = GBA_GetSystem();
  GBA_Input *input = &system->input;

  // act
  system->keypad->value = ~(GBA_KEY_B | GBA_KEY_LEFT);
  GBA_Input_PollStates();

  system->keypad->value = ~(GBA_KEY_A | GBA_KEY_DOWN);
  GBA_Input_PollStates();

  // assert
  assert(GBA_Input_IsHit(GBA_KEY_A) == true);
  assert(GBA_Input_IsHit(GBA_KEY_B) == false);
  assert(GBA_Input_IsHit(GBA_KEY_DOWN) == true);
  assert(GBA_Input_IsHit(GBA_KEY_LEFT) == false);
}

test(Input_IsHeld_ShoudReturnTrueWhenQueriedKeyWasHeldInLastKeypadPoll) {
  // arrange
  GBA_System *system = GBA_GetSystem();
  GBA_Input *input = &system->input;

  // act
  system->keypad->value = ~(GBA_KEY_B | GBA_KEY_LEFT);
  GBA_Input_PollStates();

  system->keypad->value = ~(GBA_KEY_B | GBA_KEY_LEFT);
  GBA_Input_PollStates();

  // assert
  assert(GBA_Input_IsHeld(GBA_KEY_A) == false);
  assert(GBA_Input_IsHeld(GBA_KEY_B) == true);
  assert(GBA_Input_IsHeld(GBA_KEY_DOWN) == false);
  assert(GBA_Input_IsHeld(GBA_KEY_LEFT) == true);
}

test(TileMapRef_BlitTile_ShouldWriteTileAtExpectedIndex) {
  // arrange
  GBA_EnableBackgroundLayer(0, (GBA_BackgroundControl) {
      .size = 3, // 512x512
      .colorMode = 1,
      .tileSetIndex = 0,
      .tileMapIndex = 14,
      .priority = 2,
  });

  GBA_TileMapRef layer;
  GBA_TileMapRef_FromBackgroundLayer(&layer, 0);

  GBA_Tile tile = { .tileId = 42 };

  // act
  GBA_TileMapRef_BlitTile(&layer, 10, 10, &tile);
  GBA_TileMapRef_BlitTile(&layer, 40, 20, &tile);
  GBA_TileMapRef_BlitTile(&layer, 25, 50, &tile);
  GBA_TileMapRef_BlitTile(&layer, 50, 50, &tile);

  // assert
  assert(layer.tiles[       32 * 10 + 10].value == tile.value);
  assert(layer.tiles[1024 + 32 * 20 +  8].value == tile.value);
  assert(layer.tiles[2048 + 32 * 18 + 25].value == tile.value);
  assert(layer.tiles[3072 + 32 * 18 + 18].value == tile.value);
}

test(Bitmap8_GetPixel_ShouldReturnExpectedPixels) {
  // arrange
  GBA_Color palette[] = {
    GBA_Color_From(0x000000),
    GBA_Color_From(0xFFFFFF),
    GBA_Color_From(0x100068),
    GBA_Color_From(0x78f800),
  };

  GBA_Bitmap8 bitmap = {
    0x00000000,0x00000000,0x01010100,0x01010101,0x02020101,0x02020202,0x02020201,0x02020202,
    0x02030201,0x02020202,0x02020201,0x02020202,0x02020101,0x02020202,0x01010100,0x01010101,
  };

  // act
  int pixel1 = GBA_Bitmap8_GetPixel(&bitmap, 2, 4);
  int pixel2 = GBA_Bitmap8_GetPixel(&bitmap, 7, 1);

  // assert
  assert(palette[pixel1].value == palette[3].value);
  assert(palette[pixel2].value == palette[1].value);
}

suite(
  Sprite_ShapeOf_ShouldReturnExpectedValues,
  Sprite_SizeOf_ShouldReturnExpectedValues,
  Sprite_Allocate_ShouldReturnNewSpriteInstanceForEachCall,
  Sprite_Release_ShouldMakeReleasedSpriteAvailableAgain,
  Sprite_ResetAll_ShouldMakeAllUsedSpritesAvailableAgain,
  Sprite_GetAffine_ShouldReturnNullWhenSpriteHasNoAffineAssigned,
  Sprite_GetAffine_ShouldReturnPreviouslyAssignedAffine,
  Affine_Allocate_ShouldReturnNewAffineInstanceForEachCall,
  Affine_Release_ShouldMakeReleasedAffineAvailableAgain,
  Input_IsHit_ShoudReturnTrueWhenQueriedKeyWasHitInLastKeypadPoll,
  Input_IsHeld_ShoudReturnTrueWhenQueriedKeyWasHeldInLastKeypadPoll,
  TileMapRef_BlitTile_ShouldWriteTileAtExpectedIndex,
  Bitmap8_GetPixel_ShouldReturnExpectedPixels);
