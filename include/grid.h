#ifndef GRID_H
#define GRID_H

#include <gba.h>
#include <vector.h>
#include <hit.h>

typedef struct Unit {
  Bounds *bounds;
  void *object;
} Unit;

typedef struct Cell {
  Bounds bounds;
  int size;
  struct Cell *cells[4];
  struct Unit units[8];
} Cell;

typedef struct Cell Grid;

#define Cell_Of(x, y, w, h) ((Cell) { .bounds = {{ x, y }, { w, h }} })
#define Unit_Of(bounds, object) ((Unit) { bounds, object })

bool
Cell_AddUnit(
    Cell *cell,
    Unit *unit);

void
Cell_QueryUnits(
    Cell *cell,
    Bounds *bounds);

#endif
