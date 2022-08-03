
#include <grid.h>
#include "test.h"

test(AddUnit_ShouldNotAddUnitIfUnitBoundsAreNotEntirelyContainedWithinCell) {
  // arrange
  Cell root = Cell_Of(10, 10, 5, 5);

  Bounds bound1 = Bounds_Of(13, 13, 4, 4);
  Unit unit1 = Unit_Of(&bound1, NULL);

  // act
  bool result = Cell_AddUnit(&root, &unit1);

  // assert
  assert(result == false);
}

test(AddUnit_ShouldAddUnitsToRootCellUntilCapacityIsExceeded) {
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

test(AddUnit_ShouldSubdivideRootCellWhenCapacityIsExceededAndRelocateUnits) {
  // arrange
  Cell root = Cell_Of(10, 10, 5, 5);

  Bounds bound1 = Bounds_Of(13, 13, 1, 1);
  Unit unit1 = Unit_Of(&bound1, NULL);

  Bounds bound2 = Bounds_Of(8, 8, 2, 2);
  Unit unit2 = Unit_Of(&bound2, NULL);

  Bounds bound3 = Bounds_Of(7, 13, 2, 1);
  Unit unit3 = Unit_Of(&bound3, NULL);

  Bounds bound4 = Bounds_Of(9, 12, 2, 1);
  Unit unit4 = Unit_Of(&bound4, NULL);

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
  assert(root.units[0].bounds == &bound4);
  assert(root.units[1].bounds == &bound4);

  assert(root.cells[0] != NULL);
  assert(root.cells[0]->size == 2);
  assert(root.cells[0]->units[0].bounds == &bound2);
  assert(root.cells[0]->units[1].bounds == &bound2);

  assert(root.cells[1] != NULL);
  assert(root.cells[1]->size == 2);
  assert(root.cells[1]->units[0].bounds == &bound3);
  assert(root.cells[1]->units[1].bounds == &bound3);

  assert(root.cells[2] != NULL);
  assert(root.cells[2]->size == 0);

  assert(root.cells[3] != NULL);
  assert(root.cells[3]->size == 2);
  assert(root.cells[3]->units[0].bounds == &bound1);
  assert(root.cells[3]->units[1].bounds == &bound1);
}

suite(
  AddUnit_ShouldNotAddUnitIfUnitBoundsAreNotEntirelyContainedWithinCell,
  AddUnit_ShouldAddUnitsToRootCellUntilCapacityIsExceeded,
  AddUnit_ShouldSubdivideRootCellWhenCapacityIsExceededAndRelocateUnits);
