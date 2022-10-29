
#include <game/loader.h>

Loader*
Loader_ForTestCourse() {
  static Loader loader = {
    "                                                                  x|"
    "                                                     xxxxxx       x|"
    "                                                                  x|"
    "                                                  x               x|"
    "                                                                  x|"
    "                                               x                  x|"
    "                                                                  x|"
    "                                            x                     x|"
    "                              i                                   x|"
    "                         i    x  xxxxxxxxxx                       x|"
    "                    i    x    x          xxxxx                    x|"
    "_o_______^___^^_____x____x____x____________xxxxx__________________x|"
  };

  return &loader;
}

static void
Loader_ResetCursor(Loader *loader) {
  const char *layout = loader->layout;

  int width = 0, height = 0;
  bool first = true;

  while (*layout != '\0') {
    char symbol = *layout;

    if (first) width++;
    if (symbol == '|') {
      height++;
      first = false;
    }

    layout++;
  }

  loader->size.x = width;
  loader->size.y = height; 

  loader->cursor.x = -1;
  loader->cursor.y =  0;
}

static bool
Loader_NextSymbol(
    Loader *loader,
    char *symbol)
{
  Vector *cursor = &loader->cursor;

  int x = cursor->x + 1;
  int y = cursor->y;

  Vector *size = &loader->size;
  if (x >= size->x) {
    x = 0; y++;
    if (y >= size->y) {
      return false;
    }
  }

  int index = size->x * y + x;
  *symbol = loader->layout[index];

  cursor->x = x;
  cursor->y = y;

  return true;
}

static bool
Loader_GetSymbol(
    Loader *loader,
    Direction direction,
    char *symbol)
{
  const Vector *delta = Vector_FromDirection(direction);
  Vector *cursor = &loader->cursor;

  int x = cursor->x + delta->x;
  int y = cursor->y + delta->y;

  Vector *size = &loader->size;
  if (x >= size->x || y >= size->y) {
    return false;
  }

  int index = size->x * y + x;
  *symbol = loader->layout[index];

  cursor->x = x;
  cursor->y = y;

  return true;
}

static inline void
Loader_AddObjectToCourse(
    Loader *loader,
    Course *course,
    Object *object)
{
  Bounds *bounds = &object->viewbox;
  Hit hit = Course_CheckHits(course, bounds);

  int delta = Math_abs(hit.delta.x) + Math_abs(hit.delta.y);
  if (delta < 4) {
    Object* o = Course_AllocateObject(course);
    *o = *object;
    Course_AddObject(course, o);
  }
}

static void
Loader_AddBoxWithPole(
    Loader *loader,
    Course *course)
{
  Vector cursor = loader->cursor;
  int height = 0;

  char symbol;
  while (Loader_GetSymbol(loader, DIRECTION_DOWN, &symbol)) {
    if (symbol != 'x') break;
    else height++;
  }

  loader->cursor = cursor;

  Object box = {0};
  Object_CreateBoxWithPole(&box, height);
  Object_SetPosition(&box, cursor.x * 2, cursor.y * 2 - 1);
  Loader_AddObjectToCourse(loader, course, &box);
}

static void
Loader_AddBox(
    Loader *loader,
    Course *course)
{
  Vector *cursor = &loader->cursor;

  Object box = {0};
  Object_CreateBox(&box);
  Object_SetPosition(&box, cursor->x * 2, cursor->y * 2);
  Loader_AddObjectToCourse(loader, course, &box);
}

static void
Loader_AddSpike(
    Loader *loader,
    Course *course)
{
  Vector *cursor = &loader->cursor;

  Object spike = {0};
  Object_CreateSpike(&spike, DIRECTION_UP);
  Object_SetPosition(&spike, cursor->x * 2, cursor->y * 2);
  Loader_AddObjectToCourse(loader, course, &spike);
}

static void
Loader_SetSpawn(
    Loader *loader,
    Course *course)
{
  Vector *cursor = &loader->cursor;
  // standard box is 2x2 tiles, multiply by 16 to get pixel coordinates
  Course_SetSpawn(course, cursor->x * 16, cursor->y * 16 + 8);
}

static void
Loader_SetFloorHeight(
    Loader *loader,
    Course *course)
{
  Vector *cursor = &loader->cursor;
  // standard box is 2x2 tiles, add 1 to align at bottom
  Course_SetFloorHeight(course, cursor->y * 2 + 1);
}

bool
Loader_LoadCourse(
    Loader *loader,
    Course *course)
{
  char symbol;

  Loader_ResetCursor(loader);
  Course_Reset(course);

  // workaround to not collide with floor when adding objects
  Course_SetFloorHeight(course, 100);

  while (Loader_NextSymbol(loader, &symbol)) {
    switch (symbol) {
    case 'i':
      Loader_AddBoxWithPole(loader, course);
      break;
    case 'x':
      Loader_AddBox(loader, course);
      break;
    case '^':
      Loader_AddSpike(loader, course);
      break;
    case 'o':
      Loader_SetSpawn(loader, course);
      break;
    case '_':
      Loader_SetFloorHeight(loader, course);
      break;
    }
  }

  return true;
}
