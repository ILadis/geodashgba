#ifndef SELECTOR_H
#define SELECTOR_H

#include <gba.h>

typedef struct Selector {
  bool redraw;
} Selector;

Selector*
Selector_GetInstance();

void
Selector_Update(Selector *selector);

void
Selector_Draw(Selector *selector);

#endif
