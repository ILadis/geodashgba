
#include <game/loader.h>

static void
Loader_ResetCursor(Loader *loader) {
  const char **layout = loader->layout;

  int x = 0, y = 0;

  while (layout[y++] != NULL);
  while (layout[0][x++] != '\0');

  loader->size.x = x - 1;
  loader->size.y = y - 1;

  loader->cursor.x = -1;
  loader->cursor.y =  0;
}

Loader*
Loader_ForTestCourse(bool reset) {
  static const char *layout[] = {
    "                                                                  x",
    "                                                     xxxxxx       x",
    "                                                                  x",
    "                                                  x               x",
    "                                                                  x",
    "                                               x                  x",
    "                                                                  x",
    "                                            x                     x",
    "                              i                                   x",
    "                         i    x  xxxxxxxxxx                       x",
    "                    i    x    x          xxxxx                    x",
    "_________^___^^_____x____x____x____________xxxxx__________________x",
    NULL
  };

 static Loader loader = { layout };

  if (reset) {
    Loader_ResetCursor(&loader);
  }

  return &loader;
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

  *symbol = loader->layout[y][x];

  cursor->x = x;
  cursor->y = y;

  return true;
}

static bool
Loader_GetSymbolAt(
    Loader *loader,
    int x, int y,
    char *symbol)
{
  Vector *size = &loader->size;
  if (x >= size->x || y >= size->y) {
    return false;
  }

  *symbol = loader->layout[y][x];

  Vector *cursor = &loader->cursor;

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

  *symbol = loader->layout[y][x];

  cursor->x = x;
  cursor->y = y;

  return true;
}

static inline Vector
Loader_GetCursorPosition(
    Loader *loader,
    Chunk *chunk)
{
  Vector *cursor = &loader->cursor;
  Vector *size = &loader->size;

  Vector position = {
    .x = cursor->x * 2,
    .y = cursor->y * 2,
  };

  const Bounds *bounds = Chunk_GetBounds(chunk);
  int dy = (bounds->size.y >> 2) - size->y*2;

  // allign at bottom of chunk bounds
  position.y += dy;

  return position;
}

static inline void
Loader_AddObjectToChunk(
    Loader *loader,
    Chunk *chunk,
    Object *template)
{
  Vector position = Loader_GetCursorPosition(loader, chunk);
  Object_SetPosition(template, position.x, position.y);

  Unit unit = Unit_Of(&template->hitbox, template);
  Hit hit = Chunk_CheckHits(chunk, &unit, NULL);

  if (!Hit_IsHit(&hit)) {
    Object *object = Chunk_AllocateObject(chunk);
    Object_AssignFrom(object, template);
    Chunk_AddObject(chunk, object);
  }
}

static void
Loader_AddBoxWithPole(
    Loader *loader,
    Chunk *chunk)
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
  Object_SetPosition(&box, 0, -1);
  Loader_AddObjectToChunk(loader, chunk, &box);
}

static void
Loader_AddBox(
    Loader *loader,
    Chunk *chunk)
{
  Object box = {0};
  Object_CreateBox(&box);
  Loader_AddObjectToChunk(loader, chunk, &box);
}

static void
Loader_AddSpike(
    Loader *loader,
    Chunk *chunk)
{
  Object spike = {0};
  Object_CreateSpike(&spike, DIRECTION_UP);
  Loader_AddObjectToChunk(loader, chunk, &spike);
}

void
Loader_GetChunk(
    Loader *loader,
    Chunk *chunk)
{
  const Bounds *bounds = Chunk_GetBounds(chunk);
  Vector lower = Bounds_Lower(bounds);
  Vector upper = Bounds_Upper(bounds);

  // convert to cursor coordinates
  lower.x >>= 4; lower.y >>= 4;
  upper.x >>= 4; upper.y >>= 4;

  for (int y = lower.y; y < upper.y; y++) {
    for (int x = lower.x; x < upper.x; x++) {
      char symbol;
      if (!Loader_GetSymbolAt(loader, x, y, &symbol)) {
        break; // continue with next y
      }

      switch (symbol) {
      case 'i':
        Loader_AddBoxWithPole(loader, chunk);
        break;
      case 'x':
        Loader_AddBox(loader, chunk);
        break;
      case '^':
        Loader_AddSpike(loader, chunk);
        break;
      }
    }
  }
}
