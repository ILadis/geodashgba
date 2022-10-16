
#include <gba.h>
#include "test.h"

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
  GBA_Input_PollStates(input);

  system->keypad->value = ~(GBA_KEY_A | GBA_KEY_DOWN);
  GBA_Input_PollStates(input);

  // assert
  assert(GBA_Input_IsHit(input, GBA_KEY_A) == true);
  assert(GBA_Input_IsHit(input, GBA_KEY_B) == false);
  assert(GBA_Input_IsHit(input, GBA_KEY_DOWN) == true);
  assert(GBA_Input_IsHit(input, GBA_KEY_LEFT) == false);
}

test(Input_IsHeld_ShoudReturnTrueWhenQueriedKeyWasHeldInLastKeypadPoll) {
  // arrange
  GBA_System *system = GBA_GetSystem();
  GBA_Input *input = &system->input;

  // act
  system->keypad->value = ~(GBA_KEY_B | GBA_KEY_LEFT);
  GBA_Input_PollStates(input);

  system->keypad->value = ~(GBA_KEY_B | GBA_KEY_LEFT);
  GBA_Input_PollStates(input);

  // assert
  assert(GBA_Input_IsHeld(input, GBA_KEY_A) == false);
  assert(GBA_Input_IsHeld(input, GBA_KEY_B) == true);
  assert(GBA_Input_IsHeld(input, GBA_KEY_DOWN) == false);
  assert(GBA_Input_IsHeld(input, GBA_KEY_LEFT) == true);
}

suite(
  Sprite_Allocate_ShouldReturnNewSpriteInstanceForEachCall,
  Sprite_Release_ShouldMakeReleasedSpriteAvailableAgain,
  Sprite_ResetAll_ShouldMakeAllUsedSpritesAvailableAgain,
  Sprite_GetAffine_ShouldReturnNullWhenSpriteHasNoAffineAssigned,
  Sprite_GetAffine_ShouldReturnPreviouslyAssignedAffine,
  Affine_Allocate_ShouldReturnNewAffineInstanceForEachCall,
  Affine_Release_ShouldMakeReleasedAffineAvailableAgain,
  Input_IsHit_ShoudReturnTrueWhenQueriedKeyWasHitInLastKeypadPoll,
  Input_IsHeld_ShoudReturnTrueWhenQueriedKeyWasHeldInLastKeypadPoll);
