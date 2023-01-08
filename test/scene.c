
#include <scene.h>
#include "test.h"

test(GetCurrent_ShoudReturnSameInstance) {
  // act
  Scene *scene1 = Scene_GetCurrent();
  Scene *scene2 = Scene_GetCurrent();

  // assert
  assert(scene1 == scene2);
}

static int count;

static void
Stage_SetCount() { count = 7; }

static void
Stage_ResetCount() { count = 0; }

static void
Stage_IncrementCount() { count++; }

test(Play_ShouldInvokeEnterCallback) {
  // arrange
  Scene *scene = &(Scene) {
    .enter = Stage_SetCount,
    .play = Scene_Noop,
    .exit = Scene_Noop,
  };

  // act
  Scene_Play(scene);
  Scene_Play(scene);
  Scene_Play(scene);

  // assert
  assert(count == 7);
  assert(scene->stage == STAGE_PLAY);
}

test(Play_ShouldInvokePlayCallbackUntilSceneIsAboutToBeRelaced) {
  // arrange
  Scene *scene = &(Scene) {
    .enter = Stage_ResetCount,
    .play = Stage_IncrementCount,
    .exit = Stage_ResetCount,
  };

  Scene *next = &(Scene) {
    .enter = Stage_IncrementCount,
    .play = Stage_ResetCount,
    .exit = Stage_ResetCount,
  };

  Scene_Play(scene);

  // act
  Scene_Play(scene);
  Scene_Play(scene);
  Scene_Play(scene);

  Scene_ReplaceWith(scene, next);

  Scene_Play(scene);
  Scene_Play(scene);

  // assert
  assert(count == 1);
  assert(scene->stage == STAGE_PLAY);
}

test(Play_ShouldInvokeExitCallbackWhenSceneIsBeingReplaced) {
  // arrange
  Scene *scene = &(Scene) {
    .enter = Stage_ResetCount,
    .play = Stage_IncrementCount,
    .exit = Stage_SetCount,
  };

  Scene *next = &(Scene) {
    .enter = Stage_ResetCount,
    .play = Stage_ResetCount,
    .exit = Stage_ResetCount,
  };

  Scene_Play(scene);

  // act
  Scene_Play(scene);
  Scene_Play(scene);
  Scene_Play(scene);

  Scene_ReplaceWith(scene, next);

  Scene_Play(scene);

  // assert
  assert(count == 7);
  assert(scene->stage == STAGE_ENTER);
  assert(scene->enter == next->enter);
  assert(scene->play == next->play);
  assert(scene->exit == next->exit);
}

suite(
  GetCurrent_ShoudReturnSameInstance,
  Play_ShouldInvokeEnterCallback,
  Play_ShouldInvokePlayCallbackUntilSceneIsAboutToBeRelaced,
  Play_ShouldInvokeExitCallbackWhenSceneIsBeingReplaced);
