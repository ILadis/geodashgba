
#include <grid.h>

static inline void
Cell_Reset(Cell *cell) {
  if (Cell_IsDivided(cell)) {
    Cell_Reset(cell->cells[0]);
    Cell_Reset(cell->cells[1]);
    Cell_Reset(cell->cells[2]);
    Cell_Reset(cell->cells[3]);
  }

  cell->size = 0;
  cell->cells[0] = NULL;
  cell->cells[1] = NULL;
  cell->cells[2] = NULL;
  cell->cells[3] = NULL;
}

void
Grid_Reset(Grid *grid) {
  Cell *root = Grid_GetRoot(grid);
  Cell_Reset(root);

  grid->count = 0;
}

static inline void
Cell_SetSlotBounds(
    Cell *parent,
    int slot)
{
  static const Vector directions[4] = {
    Vector_Of(-1, -1), // top left
    Vector_Of(+1, -1), // top right
    Vector_Of(-1, +1), // bottom left
    Vector_Of(+1, +1), // bottom right
  };

  const Vector *direction = &directions[slot];
  Bounds *bounds = &parent->bounds;

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
    int adjust = (slot & 0b01) == 0;
    center.x -=  adjust;
    size.x   += !adjust;
  }

  if (oddHeight) {
    int adjust = (slot & 0b10) == 0;
    center.y -=  adjust;
    size.y   += !adjust;
  }

  Cell *cell = parent->cells[slot];
  cell->bounds.size = size;
  cell->bounds.center = center;
}

static Cell*
Grid_AllocateNew(
    Grid *grid,
    Cell *parent,
    int slot)
{
  int index = grid->count++;

  Cell *cell = parent->cells[slot] = &grid->cells[index];
  Cell_SetSlotBounds(parent, slot);

  return cell;
}

static bool Grid_AddUnitToCell(Grid *grid, Cell *cell, Unit *unit);

static inline void
Grid_RelocateUnits(
    Grid *grid,
    Cell *parent,
    Cell *cell)
{
  Unit *units = parent->units;
  int count = parent->size;

  for (int i = 0, j = 0; i < count; i++) {
    Unit *unit = &units[i];

    if (Grid_AddUnitToCell(grid, cell, unit)) {
      parent->size--;
    } else {
      units[j].bounds = unit->bounds;
      units[j++].object = unit->object;
    }
  }
}

Cell*
Grid_Subdivide(
    Grid *grid,
    Cell *parent)
{
  if (Cell_IsDivided(parent)) {
    return parent->cells[0];
  }

  // this cell is to small to divide
  Vector size = parent->bounds.size;
  if (size.x <= 1 || size.y <= 1) {
    return NULL;
  }

  for (int slot = 0; slot < 4; slot++) {
    Cell *cell = Grid_AllocateNew(grid, parent, slot);
    Grid_RelocateUnits(grid, parent, cell);
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

static bool
Grid_AddUnitToCell(
    Grid *grid,
    Cell *cell,
    Unit *unit)
{
  if (!Cell_UnitInBounds(cell, unit)) {
    return false;
  }

  if (Cell_IsDivided(cell)) {
    // try add unit to most fitting sub cell
    if (Grid_AddUnitToCell(grid, cell->cells[0], unit)) return true;
    if (Grid_AddUnitToCell(grid, cell->cells[1], unit)) return true;
    if (Grid_AddUnitToCell(grid, cell->cells[2], unit)) return true;
    if (Grid_AddUnitToCell(grid, cell->cells[3], unit)) return true;
  }

  if (Cell_TryAddUnit(cell, unit)) {
    return true;
  }

  Cell *cells = Grid_Subdivide(grid, cell);
  if (cells == NULL) {
    return false;
  }

  if (Grid_AddUnitToCell(grid, &cells[0], unit)) return true;
  if (Grid_AddUnitToCell(grid, &cells[1], unit)) return true;
  if (Grid_AddUnitToCell(grid, &cells[2], unit)) return true;
  if (Grid_AddUnitToCell(grid, &cells[3], unit)) return true;

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

static void
Cell_RelocateSlotBounds(Cell *parent) {
  for (int slot = 0; slot < 4; slot++) {
    if (parent->cells[slot] != NULL) {
      Cell_SetSlotBounds(parent, slot);
      Cell_RelocateSlotBounds(parent->cells[slot]);
    }
  }
}

bool
Grid_AddUnit(
    Grid *grid,
    Unit *unit)
{
  Cell *root = &grid->root;

  if (!Cell_UnitInBounds(root, unit)) {
    const Bounds bounds = Bounds_Expand(&root->bounds, unit->bounds);
    root->bounds = bounds;
    Cell_RelocateSlotBounds(root);
  }

  return Grid_AddUnitToCell(grid, root, unit);
}

static inline int
Iterator_GetCapacity(Iterator *iterator) {
  return length(iterator->stack.cells);
}

static inline void
Iterator_PushStack(
    Iterator *iterator,
    Cell *cell)
{
  int index = iterator->stack.size;
  int capacity = Iterator_GetCapacity(iterator);

  if (index < capacity) {
    iterator->stack.cells[index] = cell;
    iterator->stack.size++;
  }
}

static inline Cell*
Iterator_PopStack(Iterator *iterator) {
  int index = iterator->stack.size - 1;

  if (index >= 0) {
    iterator->stack.size = index;
    return iterator->stack.cells[index];
  }

  return NULL;
}

static bool
Iterator_CheckNext(
    Iterator *iterator,
    Cell *cell)
{
  if (cell == NULL) {
    return false;
  }

  // there may be cases where intermediate cells are empty
  int size = cell->size;
  if (size <= 0) {
    bool divided = Cell_IsDivided(cell);

    if (divided) {
      Iterator_CheckNext(iterator, cell->cells[0]);
      Iterator_CheckNext(iterator, cell->cells[1]);
      Iterator_CheckNext(iterator, cell->cells[2]);
      Iterator_CheckNext(iterator, cell->cells[3]);
    }

    return false;
  }

  Hit hit = Bounds_Intersects(iterator->bounds, &cell->bounds);
  if (!Hit_IsHit(&hit)) {
    return false;
  }

  Iterator_PushStack(iterator, cell);

  return true;
}

Unit*
Iterator_GetNext(Iterator *iterator) {
  Cell *cell = iterator->next.cell;

  if (cell == NULL) {
    return NULL;
  }

  int index = iterator->next.index++;
  int size = cell->size;

  Unit *unit = &cell->units[index++];

  if (index >= size) {
    Iterator_CheckNext(iterator, cell->cells[0]);
    Iterator_CheckNext(iterator, cell->cells[1]);
    Iterator_CheckNext(iterator, cell->cells[2]);
    Iterator_CheckNext(iterator, cell->cells[3]);

    Cell *next = Iterator_PopStack(iterator);

    iterator->next.index = 0;
    iterator->next.cell = next;
  }

  return unit;
}

bool
Grid_GetUnits(
    Grid *grid,
    Bounds *bounds,
    Iterator *iterator)
{
  iterator->bounds = bounds;
  iterator->stack.size = 0;

  Cell *root = &grid->root;
  Iterator_CheckNext(iterator, root);

  Cell *next = Iterator_PopStack(iterator);

  iterator->next.index = 0;
  iterator->next.cell = next;

  return true;
}
