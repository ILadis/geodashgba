#ifndef SELECTOR_H
#define SELECTOR_H

#include <gba.h>
#include <text.h>
#include <hit.h>
#include <animation.h>

#include <game/level.h>

typedef struct Selector {
  LevelId id;
  Animation scroll, move;
  GBA_Sprite *arrows[2];
  Printer printer;
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
  return Level_GetById(selector->id);
}

void
Selector_Update(Selector *selector);

void
Selector_Draw(Selector *selector);

#endif
