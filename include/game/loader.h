#ifndef LOADER_H
#define LOADER_H

#include <gba.h>
#include <vector.h>

#include <game/course.h>
#include <game/object.h>

typedef struct Loader {
  const char *layout;
  Vector size, cursor;
  // int index;
  // error
} Loader;

#define Loader_ForLayout(layout) ((Loader) { layout })

Loader*
Loader_ForTestCourse();

bool
Loader_LoadCourse(
    Loader *loader,
    Course *course);

#endif
