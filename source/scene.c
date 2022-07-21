
#include <scene.h>

Scene*
Scene_GetCurrent() {
  static Scene scene = {
    .enter = Scene_Noop,
    .play = Scene_Noop,
    .exit = Scene_Noop,
  };

  return &scene;
}

static inline void
Scene_StageEnter(Scene *scene) {
  scene->enter();
  scene->stage = STAGE_PLAY;
}

static inline void
Scene_StagePlay(Scene *scene) {
  if (scene->next != NULL) {
    scene->stage = STAGE_REPLACE;
  }
  else scene->play();
}

static inline void
Scene_StageReplace(Scene *scene) {
  scene->exit();

  const Scene *next = scene->next;

  scene->enter = next->enter;
  scene->play  = next->play;
  scene->exit  = next->exit;

  scene->next = NULL;
  scene->stage = STAGE_ENTER;
}

void
Scene_Play(Scene *scene) {
  Stage stage = scene->stage;

  switch (stage) {
  case STAGE_ENTER:
    return Scene_StageEnter(scene);

  case STAGE_PLAY:
    return Scene_StagePlay(scene);

  case STAGE_REPLACE:
    return Scene_StageReplace(scene);
  }
}

void
Scene_ReplaceWith(
    Scene *scene,
    const Scene *next)
{
  scene->next = next;
}
