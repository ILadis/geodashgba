#ifndef SELECTOR_H
#define SELECTOR_H

#include <gba.h>
#include <hit.h>
#include <animation.h>

#include <game/level.h>

typedef struct Selector {
  LevelId id;
  Animation scroll;
  bool redraw;
} Selector;

Selector*
Selector_GetInstance(bool redraw);

void
Selector_GoForward(Selector *selector);

void
Selector_GoBackward(Selector *selector);

static inline Level*
Selector_GetLevel(Selector *selector) {
  return Level_GetById(selector->id);
}

void
Selector_Update(Selector *selector);

void
Selector_Draw(Selector *selector);

#endif
