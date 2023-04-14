
#include <grid.h>
#include "test.h"

test(Subdivide_ShouldSubdivideCellGaplessWhenOddCellWidthIsGiven) {
  // arrange
  Grid *grid = Grid_Of(10, 10, 5, 6, 20);
  Cell *root = Grid_GetRoot(grid);

  // act
  Cell *cells = Grid_Subdivide(grid, root);

  // assert
  assert(cells[0].bounds.center.x == 7);
  assert(cells[0].bounds.center.y == 7);
  assert(cells[0].bounds.size.x == 2);
  assert(cells[0].bounds.size.y == 3);

  assert(cells[1].bounds.center.x == 12);
  assert(cells[1].bounds.center.y == 7);
  assert(cells[1].bounds.size.x == 3);
  assert(cells[1].bounds.size.y == 3);

  assert(cells[2].bounds.center.x == 7);
  assert(cells[2].bounds.center.y == 13);
  assert(cells[2].bounds.size.x == 2);
  assert(cells[2].bounds.size.y == 3);

  assert(cells[3].bounds.center.x == 12);
  assert(cells[3].bounds.center.y == 13);
  assert(cells[3].bounds.size.x == 3);
  assert(cells[3].bounds.size.y == 3);
}

test(Subdivide_ShouldSubdivideCellGaplessWhenOddCellHeightIsGiven) {
  // arrange
  Grid *grid = Grid_Of(10, 10, 6, 5, 20);
  Cell *root = Grid_GetRoot(grid);

  // act
  Cell *cells = Grid_Subdivide(grid, root);

  // assert
  assert(cells[0].bounds.center.x == 7);
  assert(cells[0].bounds.center.y == 7);
  assert(cells[0].bounds.size.x == 3);
  assert(cells[0].bounds.size.y == 2);

  assert(cells[1].bounds.center.x == 13);
  assert(cells[1].bounds.center.y == 7);
  assert(cells[1].bounds.size.x == 3);
  assert(cells[1].bounds.size.y == 2);

  assert(cells[2].bounds.center.x == 7);
  assert(cells[2].bounds.center.y == 12);
  assert(cells[2].bounds.size.x == 3);
  assert(cells[2].bounds.size.y == 3);

  assert(cells[3].bounds.center.x == 13);
  assert(cells[3].bounds.center.y == 12);
  assert(cells[3].bounds.size.x == 3);
  assert(cells[3].bounds.size.y == 3);
}

test(Subdivide_ShouldSubdivideCellGaplessWhenOddCellWidthAndHeightIsGiven) {
  // arrange
  Grid *grid = Grid_Of(10, 10, 5, 5, 20);
  Cell *root = Grid_GetRoot(grid);

  // act
  Cell *cells = Grid_Subdivide(grid, root);

  // assert
  assert(cells[0].bounds.center.x == 7);
  assert(cells[0].bounds.center.y == 7);
  assert(cells[0].bounds.size.x == 2);
  assert(cells[0].bounds.size.y == 2);

  assert(cells[1].bounds.center.x == 12);
  assert(cells[1].bounds.center.y == 7);
  assert(cells[1].bounds.size.x == 3);
  assert(cells[1].bounds.size.y == 2);

  assert(cells[2].bounds.center.x == 7);
  assert(cells[2].bounds.center.y == 12);
  assert(cells[2].bounds.size.x == 2);
  assert(cells[2].bounds.size.y == 3);

  assert(cells[3].bounds.center.x == 12);
  assert(cells[3].bounds.center.y == 12);
  assert(cells[3].bounds.size.x == 3);
  assert(cells[3].bounds.size.y == 3);
}

test(AddUnit_ShouldAddUnitsToRootCellIfCapacityIsNotExceeded) {
  // arrange
  Grid *grid = Grid_Of(10, 10, 5, 5, 20);
  Cell *root = Grid_GetRoot(grid);

  Bounds bound1 = Bounds_Of(13, 13, 1, 1);
  Unit unit1 = Unit_Of(&bound1, NULL);

  Bounds bound2 = Bounds_Of(8, 8, 2, 2);
  Unit unit2 = Unit_Of(&bound2, NULL);

  Bounds bound3 = Bounds_Of(7, 13, 2, 1);
  Unit unit3 = Unit_Of(&bound3, NULL);

  // assert
  Grid_AddUnit(grid, &unit1);
  Grid_AddUnit(grid, &unit2);
  Grid_AddUnit(grid, &unit3);

  // assert
  assert(root->size == 3);
  assert(root->cells[0] == NULL);
}

test(AddUnit_ShouldAddUnitToMostFittingSubcell) {
  // arrange
  Grid *grid = Grid_Of(10, 10, 6, 6, 20);
  Cell *root = Grid_GetRoot(grid);

  Grid_Subdivide(grid, Grid_Subdivide(grid, root));

  Bounds bounds = Bounds_Of(5, 5, 1, 1);
  Unit unit = Unit_Of(&bounds, NULL);

  // act
  Grid_AddUnit(grid, &unit);

  // assert
  assert(root->size == 0);
  assert(root->cells[0] != NULL);

  assert(root->cells[0]->size == 0);
  assert(root->cells[0]->cells[0] != NULL);

  assert(root->cells[0]->cells[0]->size == 1);
  assert(root->cells[0]->cells[0]->cells[0] == NULL);
}

test(AddUnit_ShouldReturnFalseIfThereIsNoCapacityLeftToAddCell) {
  // arrange
  Grid *grid = Grid_Of(10, 10, 6, 5, 20);
  Cell *root = Grid_GetRoot(grid);

  Bounds bounds = Bounds_Of(13, 12, 1, 1);
  Unit unit = Unit_Of(&bounds, NULL);

  // act
  while (Grid_AddUnit(grid, &unit));

  // assert
  assert(root->size == 8);
  assert(root->cells[0] != NULL);

  assert(root->cells[3]->size == 8);
  assert(root->cells[3]->cells[0] != NULL);

  assert(root->cells[3]->cells[3]->size == 8);
  assert(root->cells[3]->cells[3]->cells[0] != NULL);

  assert(root->cells[3]->cells[3]->cells[0]->size == 8);
  assert(root->cells[3]->cells[3]->cells[0]->cells[0] == NULL);
}

test(AddUnit_ShouldSubdivideCellsWhenCapacityIsExceededAndRelocateUnits) {
  // arrange
  Grid *grid = Grid_Of(10, 10, 6, 6, 20);
  Cell *root = Grid_GetRoot(grid);

  Bounds bounds1 = Bounds_Of(13, 13, 1, 1);
  Unit unit1 = Unit_Of(&bounds1, NULL);

  Bounds bounds2 = Bounds_Of(8, 8, 2, 2);
  Unit unit2 = Unit_Of(&bounds2, NULL);

  Bounds bounds3 = Bounds_Of(13, 7, 2, 1);
  Unit unit3 = Unit_Of(&bounds3, NULL);

  Bounds bounds4 = Bounds_Of(9, 12, 2, 1);
  Unit unit4 = Unit_Of(&bounds4, NULL);

  // act
  Grid_AddUnit(grid, &unit1);
  Grid_AddUnit(grid, &unit2);
  Grid_AddUnit(grid, &unit3);
  Grid_AddUnit(grid, &unit4);
  Grid_AddUnit(grid, &unit1);
  Grid_AddUnit(grid, &unit2);
  Grid_AddUnit(grid, &unit3);
  Grid_AddUnit(grid, &unit4);
  Grid_AddUnit(grid, &unit1);

  // assert
  assert(root->size == 2);
  assert(root->units[0].bounds == &bounds4);
  assert(root->units[1].bounds == &bounds4);

  assert(root->cells[0] != NULL);
  assert(root->cells[0]->size == 2);
  assert(root->cells[0]->units[0].bounds == &bounds2);
  assert(root->cells[0]->units[1].bounds == &bounds2);

  assert(root->cells[1] != NULL);
  assert(root->cells[1]->size == 2);
  assert(root->cells[1]->units[0].bounds == &bounds3);
  assert(root->cells[1]->units[1].bounds == &bounds3);

  assert(root->cells[2] != NULL);
  assert(root->cells[2]->size == 0);

  assert(root->cells[3] != NULL);
  assert(root->cells[3]->size == 3);
  assert(root->cells[3]->units[0].bounds == &bounds1);
  assert(root->cells[3]->units[1].bounds == &bounds1);
  assert(root->cells[3]->units[2].bounds == &bounds1);
}

test(AddUnit_ShouldEnlargeGridIfUnitBoundsAreNotEntirelyContainedWithinRootCell) {
  // arrange
  Grid *grid = Grid_Of(10, 10, 5, 5, 20);
  Cell *root = Grid_GetRoot(grid);

  Grid_Subdivide(grid, Grid_Subdivide(grid, root));

  Bounds bounds = Bounds_Of(13, 13, 4, 4);
  Unit unit = Unit_Of(&bounds, NULL);

  // act
  bool result = Grid_AddUnit(grid, &unit);

  // assert
  assert(result == true);
  assert(root->bounds.center.x == 11);
  assert(root->bounds.center.y == 11);
  assert(root->bounds.size.x == 6);
  assert(root->bounds.size.y == 6);

  assert(root->cells[0]->bounds.center.x == 8);
  assert(root->cells[0]->bounds.center.y == 8);
  assert(root->cells[0]->bounds.size.x == 3);
  assert(root->cells[0]->bounds.size.y == 3);

  assert(root->cells[0]->cells[0]->bounds.center.x == 6);
  assert(root->cells[0]->cells[0]->bounds.center.y == 6);
  assert(root->cells[0]->cells[0]->bounds.size.x == 1);
  assert(root->cells[0]->cells[0]->bounds.size.y == 1);
}

test(GetUnits_ShouldReturnQueriedUnitsInInsertionOrder) {
  // arrange
  Grid *grid = Grid_Of(10, 10, 6, 6, 20);

  int *object1 = (int *) 0xBEEF;
  Bounds bounds1 = Bounds_Of(13, 13, 1, 1);
  Unit unit1 = Unit_Of(&bounds1, object1);

  int *object2 = (int *) 0xCAFE;
  Bounds bounds2 = Bounds_Of(8, 8, 2, 2);
  Unit unit2 = Unit_Of(&bounds2, object2);

  int *object3 = (int *) 0xDEAD;
  Bounds bounds3 = Bounds_Of(13, 7, 2, 1);
  Unit unit3 = Unit_Of(&bounds3, object3);

  Grid_AddUnit(grid, &unit1);
  Grid_AddUnit(grid, &unit2);
  Grid_AddUnit(grid, &unit3);

  Unit *next = NULL;
  Bounds query = Bounds_Of(12, 12, 8, 8);

  // act
  Iterator iterator;
  Grid_GetUnits(grid, &query, &iterator);

  // assert
  next = Iterator_GetNext(&iterator);
  assert(next->object == object1);

  next = Iterator_GetNext(&iterator);
  assert(next->object == object2);

  next = Iterator_GetNext(&iterator);
  assert(next->object == object3);

  next = Iterator_GetNext(&iterator);
  assert(next == NULL);
}

test(GetUnits_ShouldReturnUnitsOfAllQueriedCells) {
  // arrange
  Grid *grid = Grid_Of(10, 10, 8, 8, 20);
  Cell *root = Grid_GetRoot(grid);

  Grid_Subdivide(grid, Grid_Subdivide(grid, Grid_Subdivide(grid, root)));

  int *object1 = (int *) 1;
  Bounds bounds1 = Bounds_Of(5, 5, 1, 1);
  Unit unit1 = Unit_Of(&bounds1, object1);

  int *object2 = (int *) 2;
  Bounds bounds2 = Bounds_Of(4, 4, 1, 1);
  Unit unit2 = Unit_Of(&bounds2, object2);

  int *object3 = (int *) 3;
  Bounds bounds3 = Bounds_Of(8, 4, 1, 1);
  Unit unit3 = Unit_Of(&bounds3, object3);

  int *object4 = (int *) 4;
  Bounds bounds4 = Bounds_Of(4, 8, 1, 1);
  Unit unit4 = Unit_Of(&bounds4, object4);

  int *object5 = (int *) 5;
  Bounds bounds5 = Bounds_Of(8, 8, 1, 1);
  Unit unit5 = Unit_Of(&bounds5, object5);

  int *object6 = (int *) 6;
  Bounds bounds6 = Bounds_Of(10, 10, 1, 1);
  Unit unit6 = Unit_Of(&bounds6, object6);

  Grid_AddUnit(grid, &unit1);
  Grid_AddUnit(grid, &unit2);
  Grid_AddUnit(grid, &unit3);
  Grid_AddUnit(grid, &unit4);
  Grid_AddUnit(grid, &unit5);
  Grid_AddUnit(grid, &unit6);

  Unit *next = NULL;
  Bounds query = Bounds_Of(7, 7, 3, 2);

  // act
  Iterator iterator;
  Grid_GetUnits(grid, &query, &iterator);

  // assert
  next = Iterator_GetNext(&iterator);
  assert(next->object == object6);

  next = Iterator_GetNext(&iterator);
  assert(next->object == object5);

  next = Iterator_GetNext(&iterator);
  assert(next->object == object4);

  next = Iterator_GetNext(&iterator);
  assert(next->object == object3);

  next = Iterator_GetNext(&iterator);
  assert(next->object == object2);

  next = Iterator_GetNext(&iterator);
  assert(next->object == object1);

  next = Iterator_GetNext(&iterator);
  assert(next == NULL);
}

suite(
  Subdivide_ShouldSubdivideCellGaplessWhenOddCellWidthIsGiven,
  Subdivide_ShouldSubdivideCellGaplessWhenOddCellHeightIsGiven,
  Subdivide_ShouldSubdivideCellGaplessWhenOddCellWidthAndHeightIsGiven,
  AddUnit_ShouldAddUnitsToRootCellIfCapacityIsNotExceeded,
  AddUnit_ShouldAddUnitToMostFittingSubcell,
  AddUnit_ShouldReturnFalseIfThereIsNoCapacityLeftToAddCell,
  AddUnit_ShouldSubdivideCellsWhenCapacityIsExceededAndRelocateUnits,
  AddUnit_ShouldEnlargeGridIfUnitBoundsAreNotEntirelyContainedWithinRootCell,
  GetUnits_ShouldReturnQueriedUnitsInInsertionOrder,
  GetUnits_ShouldReturnUnitsOfAllQueriedCells);
