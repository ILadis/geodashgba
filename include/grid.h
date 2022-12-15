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

typedef struct Grid {
  Cell root;
  Cell *cells;
  int count;
} Grid;

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

#define Grid_Of(x, y, w, h, size) &((Grid) { .root = { .bounds = {{ x, y }, { w, h }} }, .cells = (Cell[size]) { }, .count = 0 })
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

static inline Cell*
Grid_GetRoot(Grid *grid) {
  return &grid->root;
}

Cell*
Grid_Subdivide(
    Grid *grid,
    Cell *cell);

bool
Grid_AddUnit(
    Grid *grid,
    Unit *unit);

bool
Grid_GetUnits(
    Grid *grid,
    Bounds *bounds,
    Iterator *iterator);

static inline bool
Iterator_HasNext(Iterator *iterator) {
  return iterator->next.cell != NULL;
}

Unit*
Iterator_GetNext(Iterator *iterator);

#endif
