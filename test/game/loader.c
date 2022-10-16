
#include <game/loader.h>
#include "../test.h"

test(LoadCourse_ShouldSetExpectedCourseSize) {
  // arrange
  Course *course = Course_GetInstance();
  Loader loader = Loader_ForLayout(""
    "        |"
    "        |"
    "        |");

  // act
  Loader_LoadCourse(&loader, course);

  // assert
  assert(loader.size.x == 9);
  assert(loader.size.y == 3);
}

test(LoadCourse_ShouldSetExpectedCourseFloorHeight) {
  // arrange
  Course *course = Course_GetInstance();
  Loader loader = Loader_ForLayout(""
    "        |"
    "        |"
    "        |"
    "        |"
    "________|");

  // act
  Loader_LoadCourse(&loader, course);

  // assert
  assert(course->floor == 9);
}

test(LoadCourse_ShouldCreatedBoxesAtExpectedPositions) {
  // arrange
  Course *course = Course_GetInstance();
  Loader loader = Loader_ForLayout(""
    "x     x|"
    "   x   |"
    "x_____x|");

  // act
  Loader_LoadCourse(&loader, course);

  // assert
  assert(course->count == 5);
  assert(course->objects[0].hitbox.center.x ==  8);
  assert(course->objects[0].hitbox.center.y ==  8);
  assert(course->objects[1].hitbox.center.x ==104);
  assert(course->objects[1].hitbox.center.y ==  8);
  assert(course->objects[2].hitbox.center.x == 56);
  assert(course->objects[2].hitbox.center.y == 24);
  assert(course->objects[3].hitbox.center.x ==  8);
  assert(course->objects[3].hitbox.center.y == 40);
  assert(course->objects[4].hitbox.center.x ==104);
  assert(course->objects[4].hitbox.center.y == 40);
}

suite(
  LoadCourse_ShouldSetExpectedCourseSize,
  LoadCourse_ShouldSetExpectedCourseFloorHeight,
  LoadCourse_ShouldCreatedBoxesAtExpectedPositions);