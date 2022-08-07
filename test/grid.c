
#include <grid.h>
#include "test.h"

test(Subdivide_ShouldSubdivideCellGaplessWhenOddCellWidthIsGiven) {
  // arrange
  Cell root = Cell_Of(10, 10, 5, 6);

  // act
  Cell *cells = Cell_Subdivide(&root);

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
  Cell root = Cell_Of(10, 10, 6, 5);

  // act
  Cell *cells = Cell_Subdivide(&root);

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
  Cell root = Cell_Of(10, 10, 5, 5);

  // act
  Cell *cells = Cell_Subdivide(&root);

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

test(AddUnit_ShouldNotAddUnitIfUnitBoundsAreNotEntirelyContainedWithinCell) {
  // arrange
  Cell root = Cell_Of(10, 10, 5, 5);

  Bounds bounds = Bounds_Of(13, 13, 4, 4);
  Unit unit = Unit_Of(&bounds, NULL);

  // act
  bool result = Cell_AddUnit(&root, &unit);

  // assert
  assert(result == false);
}

test(AddUnit_ShouldAddUnitsToRootCellIfCapacityIsNotExceeded) {
  // arrange
  Cell root = Cell_Of(10, 10, 5, 5);

  Bounds bound1 = Bounds_Of(13, 13, 1, 1);
  Unit unit1 = Unit_Of(&bound1, NULL);

  Bounds bound2 = Bounds_Of(8, 8, 2, 2);
  Unit unit2 = Unit_Of(&bound2, NULL);

  Bounds bound3 = Bounds_Of(7, 13, 2, 1);
  Unit unit3 = Unit_Of(&bound3, NULL);

  // assert
  Cell_AddUnit(&root, &unit1);
  Cell_AddUnit(&root, &unit2);
  Cell_AddUnit(&root, &unit3);

  // assert
  assert(root.size == 3);
  assert(root.cells[0] == NULL);
}

test(AddUnit_ShouldAddUnitToMostFittingSubcell) {
  // arrange
  Cell root = Cell_Of(10, 10, 6, 6);
  Cell_Subdivide(Cell_Subdivide(&root));

  Bounds bounds = Bounds_Of(5, 5, 1, 1);
  Unit unit = Unit_Of(&bounds, NULL);

  // act
  Cell_AddUnit(&root, &unit);

  // assert
  assert(root.size == 0);
  assert(root.cells[0] != NULL);

  assert(root.cells[0]->size == 0);
  assert(root.cells[0]->cells[0] != NULL);

  assert(root.cells[0]->cells[0]->size == 1);
  assert(root.cells[0]->cells[0]->cells[0] == NULL);
}

test(AddUnit_ShouldReturnFalseIfThereIsNoCapacityLeftToAddCell) {
  // arrange
  Cell root = Cell_Of(10, 10, 6, 5);

  Bounds bounds = Bounds_Of(13, 12, 1, 1);
  Unit unit = Unit_Of(&bounds, NULL);

  // act
  while (Cell_AddUnit(&root, &unit));

  // assert
  assert(root.size == 8);
  assert(root.cells[0] != NULL);

  assert(root.cells[3]->size == 8);
  assert(root.cells[3]->cells[0] != NULL);

  assert(root.cells[3]->cells[3]->size == 8);
  assert(root.cells[3]->cells[3]->cells[0] != NULL);

  assert(root.cells[3]->cells[3]->cells[0]->size == 8);
  assert(root.cells[3]->cells[3]->cells[0]->cells[0] == NULL);
}

test(AddUnit_ShouldSubdivideRootCellWhenCapacityIsExceededAndRelocateUnits) {
  // arrange
  Cell root = Cell_Of(10, 10, 6, 6);

  Bounds bounds1 = Bounds_Of(13, 13, 1, 1);
  Unit unit1 = Unit_Of(&bounds1, NULL);

  Bounds bounds2 = Bounds_Of(8, 8, 2, 2);
  Unit unit2 = Unit_Of(&bounds2, NULL);

  Bounds bounds3 = Bounds_Of(13, 7, 2, 1);
  Unit unit3 = Unit_Of(&bounds3, NULL);

  Bounds bounds4 = Bounds_Of(9, 12, 2, 1);
  Unit unit4 = Unit_Of(&bounds4, NULL);

  // assert
  Cell_AddUnit(&root, &unit1);
  Cell_AddUnit(&root, &unit2);
  Cell_AddUnit(&root, &unit3);
  Cell_AddUnit(&root, &unit4);
  Cell_AddUnit(&root, &unit1);
  Cell_AddUnit(&root, &unit2);
  Cell_AddUnit(&root, &unit3);
  Cell_AddUnit(&root, &unit4);
  Cell_AddUnit(&root, &unit1);

  // assert
  assert(root.size == 2);
  assert(root.units[0].bounds == &bounds4);
  assert(root.units[1].bounds == &bounds4);

  assert(root.cells[0] != NULL);
  assert(root.cells[0]->size == 2);
  assert(root.cells[0]->units[0].bounds == &bounds2);
  assert(root.cells[0]->units[1].bounds == &bounds2);

  assert(root.cells[1] != NULL);
  assert(root.cells[1]->size == 2);
  assert(root.cells[1]->units[0].bounds == &bounds3);
  assert(root.cells[1]->units[1].bounds == &bounds3);

  assert(root.cells[2] != NULL);
  assert(root.cells[2]->size == 0);

  assert(root.cells[3] != NULL);
  assert(root.cells[3]->size == 3);
  assert(root.cells[3]->units[0].bounds == &bounds1);
  assert(root.cells[3]->units[1].bounds == &bounds1);
  assert(root.cells[3]->units[2].bounds == &bounds1);
}

test(GetUnits_ShouldReturnQueriedUnitsInInsertionOrder) {
  // arrange
  Cell root = Cell_Of(10, 10, 6, 6);

  int object1 = 0xBEEF;
  Bounds bounds1 = Bounds_Of(13, 13, 1, 1);
  Unit unit1 = Unit_Of(&bounds1, &object1);

  int object2 = 0xCAFE;
  Bounds bounds2 = Bounds_Of(8, 8, 2, 2);
  Unit unit2 = Unit_Of(&bounds2, &object2);

  int object3 = 0xDEAD;
  Bounds bounds3 = Bounds_Of(13, 7, 2, 1);
  Unit unit3 = Unit_Of(&bounds3, &object3);

  Cell_AddUnit(&root, &unit1);
  Cell_AddUnit(&root, &unit2);
  Cell_AddUnit(&root, &unit3);

  Unit *next = NULL;
  Bounds query = Bounds_Of(12, 12, 8, 8);

  // act
  Iterator iterator;
  Cell_GetUnits(&root, &query, &iterator);

  // assert
  next = Iterator_GetNext(&iterator);
  assert(next->object == &object1);

  next = Iterator_GetNext(&iterator);
  assert(next->object == &object2);

  next = Iterator_GetNext(&iterator);
  assert(next->object == &object3);

  next = Iterator_GetNext(&iterator);
  assert(next == NULL);
}

suite(
  Subdivide_ShouldSubdivideCellGaplessWhenOddCellWidthIsGiven,
  Subdivide_ShouldSubdivideCellGaplessWhenOddCellHeightIsGiven,
  Subdivide_ShouldSubdivideCellGaplessWhenOddCellWidthAndHeightIsGiven,
  AddUnit_ShouldNotAddUnitIfUnitBoundsAreNotEntirelyContainedWithinCell,
  AddUnit_ShouldAddUnitsToRootCellIfCapacityIsNotExceeded,
  AddUnit_ShouldAddUnitToMostFittingSubcell,
  AddUnit_ShouldReturnFalseIfThereIsNoCapacityLeftToAddCell,
  AddUnit_ShouldSubdivideRootCellWhenCapacityIsExceededAndRelocateUnits,
  GetUnits_ShouldReturnQueriedUnitsInInsertionOrder);
