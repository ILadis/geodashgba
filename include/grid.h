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

typedef struct Iterator {
  Bounds *bounds;
  struct {
    int size;
    Cell *cells[20];
  } stack;
  struct {
    int index;
    Cell *cell;
  } next;
} Iterator;

#define Cell_Of(x, y, w, h) ((Cell) { .bounds = {{ x, y }, { w, h }} })
#define Unit_Of(bounds, object) ((Unit) { bounds, object })

static inline bool
Cell_IsDivided(Cell *cell) {
  return cell->cells[0] != NULL;
}

static inline int
Cell_GetSize(Cell *cell) {
  return cell->size;
}

static inline int
Cell_GetCapacity(Cell *cell) {
  return ARRAY_LENGTH(cell->units);
}

Cell*
Cell_Subdivide(Cell *cell);

bool
Cell_AddUnit(
    Cell *cell,
    Unit *unit);

bool
Cell_GetUnits(
    Cell *cell,
    Bounds *bounds,
    Iterator *iterator);

static inline bool
Iterator_HasNext(Iterator *iterator) {
  return iterator->next.cell != NULL;
}

Unit*
Iterator_GetNext(Iterator *iterator);

#endif
