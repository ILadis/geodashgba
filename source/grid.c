
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

  const Vector *direction = &directions[offset];

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

  bool oddWidth  = (bounds->size.x & 0x1) == 1;
  bool oddHeight = (bounds->size.y & 0x1) == 1;

  if (oddWidth) {
    int adjust = (offset & 0b01) == 0;
    center.x -=  adjust;
    size.x   += !adjust;
  }

  if (oddHeight) {
    int adjust = (offset & 0b10) == 0;
    center.y -=  adjust;
    size.y   += !adjust;
  }

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

bool
Cell_IsDivided(Cell *parent) {
  return parent->cells[0] != NULL;
}

Cell*
Cell_Subdivide(Cell *parent) {
  if (Cell_IsDivided(parent)) {
    return parent->cells[0];
  }

  // this cell is to small to divide
  Vector size = parent->bounds.size;
  if (size.x <= 1 || size.y <= 1) {
    return NULL;
  }

  for (int i = 0; i < 4; i++) {
    Cell *cell = Cell_AllocateNew(parent, i);
    Cell_RelocateUnits(parent, cell);
  }

  return parent->cells[0];
}

static inline bool
Cell_TryAddUnit(
    Cell *cell,
    Unit *unit)
{
  int capacity = Cell_GetCapacity(cell);

  if (cell->size < capacity) {
    int index = cell->size++;
    Unit *target = &cell->units[index];

    target->bounds = unit->bounds;
    target->object = unit->object;

    return true;
  }

  return false;
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

  if (Cell_TryAddUnit(cell, unit)) {
    return true;
  }

  Cell *cells = Cell_Subdivide(cell);
  if (cells == NULL) {
    return false;
  }

  if (Cell_AddUnit(&cells[0], unit)) return true;
  if (Cell_AddUnit(&cells[1], unit)) return true;
  if (Cell_AddUnit(&cells[2], unit)) return true;
  if (Cell_AddUnit(&cells[3], unit)) return true;

  // there may now be space after relocation
  if (Cell_TryAddUnit(cell, unit)) {
    return true;
  }

  /* If we just divided this cell and could not insert given unit the newly created
   * sub sells are too small. We can free those sub cells since they are not used anyway.
   */

  // TODO free unused cells

  return false;
}

void
Cell_QueryUnits(
    Cell *cell,
    Bounds *bounds)
{
  // TODO implement this
}
