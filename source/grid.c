
#include <grid.h>
#include <stdio.h>

static Cell*
Cell_AllocateNew(
    Cell *parent,
    int offset)
{
  static int count = 0;
  static Cell cells[100] = {0};

  static const Vector directions[4] = {
    Vector_Of(-1, -1), // top left
    Vector_Of(+1, -1), // top right
    Vector_Of(-1, +1), // bottom left
    Vector_Of(+1, +1), // bottom right
  };

  if (offset < 0 || offset > 3) {
    return NULL;
  }

  const Vector *direction = &direction[offset];

  int index = count++;
  Cell *cell = &cells[index];

  Bounds *bounds = &parent->bounds;

  // TODO account for uneven sizes and positions
  Vector size = {
    .x = bounds->size.x >> 1,
    .y = bounds->size.y >> 1,
  };

  Vector center = {
    .x = bounds->center.x + size.x * direction->x,
    .y = bounds->center.y + size.y * direction->y,
  };

  cell->bounds.size = size;
  cell->bounds.center = center;

  parent->cells[offset] = cell;

  return cell;
}

static inline int
Cell_GetCapacity(Cell *cell) {
  return ARRAY_LENGTH(cell->units);
}

static inline void
Cell_SetBounds(
    Cell *cell)
{
    Bounds *bounds = &cell->bounds;

  Vector size = {
    .x = bounds->size.x >> 1,
    .y = bounds->size.y >> 1,
  };
}

static inline void
Cell_RelocateUnits(
    Cell *parent,
    Cell *cell)
{
  bool Cell_AddUnit(Grid *grid, Unit *unit);

  Unit *units = &parent->units[0];
  int count = parent->size;

  for (int i = 0; i < count; i++) {
    Unit *unit = &parent->units[i];

    if (!Cell_AddUnit(cell, unit)) {
      units->bounds = unit->bounds;
      units->object = unit->object;

      units++;
    } else {
      parent->size--;
    }
  }
}

static inline Cell*
Cell_Subdivide(Cell *parent) {
  Cell *cells = parent->cells[0];
  if (cells != NULL) {
    return cells;
  }

  for (int i = 0; i < 4; i++) {
    Cell *cell = Cell_AllocateNew(parent, i);
    Cell_RelocateUnits(parent, cell);
  }

  return parent->cells[0];
}

static inline Unit*
Cell_NextFreeUnit(Cell *cell) {
  int capacity = Cell_GetCapacity(cell);

  Unit *unit = NULL;
  if (cell->size < capacity) {
    int index = cell->size++;
    unit = &cell->units[index];
  }

  return unit;
}

static inline bool
Cell_UnitInBounds(
    Cell *cell,
    Unit *unit)
{
  Bounds embed = Bounds_Embed(&cell->bounds, unit->bounds);
  return Bounds_Equals(&embed, unit->bounds);
}

bool
Cell_AddUnit(
    Cell *cell,
    Unit *unit)
{
  if (!Cell_UnitInBounds(cell, unit)) {
    return false;
  }

  Unit *next = Cell_NextFreeUnit(cell);
  if (next != NULL) {
    next->bounds = unit->bounds;
    next->object = unit->object;
    return true;
  }

  Cell *cells = Cell_Subdivide(cell);

  if (Cell_AddUnit(&cells[0], unit)) return true;
  if (Cell_AddUnit(&cells[1], unit)) return true;
  if (Cell_AddUnit(&cells[2], unit)) return true;
  if (Cell_AddUnit(&cells[3], unit)) return true;

  return false;
}

void
Cell_QueryUnits(
    Cell *cell,
    Bounds *bounds)
{
  // TODO implement this
}
