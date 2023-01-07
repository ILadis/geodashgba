
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
  scene->play();
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

static inline void
Scene_StageFadeout(Scene *scene) {
  static const GBA_BlendControl blend = {
    .blendingMode = 3, // blend to black
    .aBG0 = 1,
    .aBG1 = 1,
    .aBG2 = 1,
    .aBG3 = 1,
    .aOBJ = 1,
  };

  GBA_System *system = GBA_GetSystem();
  int fade = scene->fade++;

  if (fade == 0) {
    system->blendControl->value = blend.value;
    system->blendFace->ey = 0;
  }

  else if (fade == 0b11111) {
    scene->exit();

    const Scene *next = scene->next;

    scene->enter = next->enter;
    scene->play  = next->play;
    scene->exit  = next->exit;

    scene->next = NULL;
    scene->stage = STAGE_FADEIN;
  }

  else {
    scene->play();
    system->blendFace->ey = fade;
  }
}

static inline void
Scene_StageFadein(Scene *scene) {
  GBA_System *system = GBA_GetSystem();
  int fade = --scene->fade;

  if (fade == 0b11111) {
    scene->enter();
  }

  else if (fade == 0) {
    scene->stage = STAGE_PLAY;
    scene->play();
  }

  else {
    scene->play();
    system->blendFace->ey = fade;
  }
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

  case STAGE_FADEOUT:
    return Scene_StageFadeout(scene);

  case STAGE_FADEIN:
    return Scene_StageFadein(scene);
  }
}

void
Scene_ReplaceWith(
    Scene *scene,
    const Scene *next)
{
  scene->stage = STAGE_REPLACE;
  scene->next = next;
}

void
Scene_FadeReplaceWith(
    Scene *scene,
    const Scene *next)
{
  scene->fade = 0;
  scene->stage = STAGE_FADEOUT;
  scene->next = next;
}
