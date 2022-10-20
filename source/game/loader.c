
#include <game/loader.h>

Loader*
Loader_ForTestCourse() {
  static Loader loader = {
    "                                                             x|"
    "                                                xxxxxx       x|"
    "                                                             x|"
    "                                             x               x|"
    "                                                             x|"
    "                                          x                  x|"
    "                                                             x|"
    "                                       x                     x|"
    "                         i                                   x|"
    "                    i    x  xxxxxxxxxx                       x|"
    "               i    x    x          xxxxx                    x|"
    "_o_____________x____x____x____________xxxxx__________________x|"
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

static void
Loader_AddObject(
    Loader *loader,
    Course *course)
{
  Vector *cursor = &loader->cursor;

  Object* object = Course_AddObject(course);
  Object_CreateBox(object);
  // TODO should consider object viewport when calculating top left position
  Object_SetPosition(object, cursor->x * 2, cursor->y * 2);
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
  Loader_ResetCursor(loader);
  Course_Reset(course);

  char symbol;
  while (Loader_NextSymbol(loader, &symbol)) {
    switch (symbol) {
    case 'x':
      Loader_AddObject(loader, course);
      break;
    case 'o':
      Loader_SetSpawn(loader, course);
      break;
    case '_':
      Loader_SetFloorHeight(loader, course);
      break;
    }
  }

  Course_Finalize(course);

  return true;
}
