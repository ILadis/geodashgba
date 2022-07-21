#ifndef SCENE_H
#define SCENE_H

#include <types.h>

typedef enum Stage {
  STAGE_ENTER,
  STAGE_PLAY,
  STAGE_REPLACE,
} Stage;

typedef struct Scene {
  void (*enter)(void);
  void (*play)(void);
  void (*exit)(void);
  const struct Scene *next;
  enum Stage stage;
} Scene;

Scene*
Scene_GetCurrent();

void
Scene_Play(Scene *scene);

void
Scene_ReplaceWith(
    Scene *scene,
    const Scene *next);

static inline void
Scene_Noop() { }

#endif
