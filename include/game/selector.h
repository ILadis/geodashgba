#ifndef SELECTOR_H
#define SELECTOR_H

#include <gba.h>
#include <text.h>
#include <hit.h>
#include <animation.h>

#include <game/level.h>
#include <game/collection.h>

typedef struct Selector {
  Animation scroll, move;
  GBA_Sprite *arrows[2];
  Printer printer;
  int index;
  bool redraw;
} Selector;

Selector*
Selector_GetInstance();

void
Selector_SetVisible(
    Selector *selector,
    bool visible);

void
Selector_GoForward(Selector *selector);

void
Selector_GoBackward(Selector *selector);

static inline Level*
Selector_GetLevel(Selector *selector) {
  const Collection *collection = Collection_GetInstance();
  Binv1Level *level = Collection_GetLevelByIndex(collection, selector->index);
  return &level->base;
}

static inline LevelId
Selector_GetLevelId(Selector *selector) {
  return Level_GetId(Selector_GetLevel(selector));
}

void
Selector_Update(Selector *selector);

void
Selector_Draw(Selector *selector);

#endif
