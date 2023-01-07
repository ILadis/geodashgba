#ifndef SELECTOR_H
#define SELECTOR_H

#include <gba.h>
#include <hit.h>

typedef struct Selector {
  int offset;
  bool redraw;
} Selector;

Selector*
Selector_GetInstance();

void
Selector_Update(Selector *selector);

void
Selector_Draw(Selector *selector);

#endif
