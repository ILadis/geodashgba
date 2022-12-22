
#include <game/loader.h>

Loader*
Loader_ForTestCourse() {
  static const char *layout[] = {
    "                                                                                                      x",
    "                                                                                                      x",
    "                                                                                                      x",
    "                                                                                                      x",
    "                                                                                                      x",
    "                                                                                                      x",
    "                                                                                                      x",
    "                                                                                                   x  x",
    "                                    i                  <x                                      x      x",
    "                               i    x                  <x                        ^         x          x",
    "                          i    x    x                             ^        xxxxxxxxxxxxx              x",
    "_________^______^^_______^x____x____x__________^___xxxxxxxx__xxxxxxxxxxx___xxxxxxxxxxxxx______________x",
  };

  static Loader loader = Loader_ForLayout(layout);
  return &loader;
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
  if (*symbol == '\0') {
    return false;
  }

  Vector *cursor = &loader->cursor;

  cursor->x = x;
  cursor->y = y;

  return true;
}

static bool
Loader_NextSymbol(
    Loader *loader,
    Direction direction,
    char *symbol)
{
  const Vector *delta = Vector_FromDirection(direction);
  Vector *cursor = &loader->cursor;

  int x = cursor->x + delta->x;
  int y = cursor->y + delta->y;

  return Loader_GetSymbolAt(loader, x, y, symbol);
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
  Object_Move(template, &position);

  Unit unit = Unit_Of(&template->hitbox, template);
  Hit hit = Chunk_CheckHits(chunk, &unit, NULL);

  if (!Hit_IsHit(&hit)) {
    Object *object = Chunk_AllocateObject(chunk);
    if (object != NULL) {
      Object_AssignFrom(object, template);
      Chunk_AddObject(chunk, object);
    }
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
  while (Loader_NextSymbol(loader, DIRECTION_DOWN, &symbol)) {
    if (symbol != 'x') break;
    else height++;
  }

  loader->cursor = cursor;

  Object object = {0};
  Vector offset = Vector_Of(0, -1);

  if (Object_CreateBoxWithPole(&object, height)) {
    Object_Move(&object, &offset);
    Loader_AddObjectToChunk(loader, chunk, &object);
  }
}

static void
Loader_AddBox(
    Loader *loader,
    Chunk *chunk)
{
  Vector cursor = loader->cursor;
  int width = 1, height = 1;

  char symbol;
  while (Loader_NextSymbol(loader, DIRECTION_RIGHT, &symbol)) {
    if (symbol != 'x') break;
    else width++;
  }

  loader->cursor = cursor;

  while (Loader_NextSymbol(loader, DIRECTION_DOWN, &symbol)) {
    if (symbol != 'x') break;

    int length = 1;
    while (Loader_NextSymbol(loader, DIRECTION_RIGHT, &symbol)) {
      if (symbol != 'x') break;
      else length++;
    }

    if (length == width) height++;
    else break;

    loader->cursor.x = cursor.x;
  }

  loader->cursor = cursor;

  Object object = {0};
  if (Object_CreateBox(&object, width, height)) {
    Loader_AddObjectToChunk(loader, chunk, &object);
  }
}

static void
Loader_AddSpike(
    Loader *loader,
    Chunk *chunk,
    Direction direction)
{
  Object object = {0};
  if (Object_CreateSpike(&object, direction)) {
    Loader_AddObjectToChunk(loader, chunk, &object);
  }
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
  Vector_Rshift(&lower, 4);
  Vector_Rshift(&upper, 4);

  // limit bounds of cursor to this chunk
  loader->size.x = upper.x;

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
        Loader_AddSpike(loader, chunk, DIRECTION_UP);
        break;
      case '<':
        Loader_AddSpike(loader, chunk, DIRECTION_LEFT);
        break;
      case '>':
        Loader_AddSpike(loader, chunk, DIRECTION_RIGHT);
        break;
      }
    }
  }
}
