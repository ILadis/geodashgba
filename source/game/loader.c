
#include <game/loader.h>

static Loader loader = Loader_ForLayout(
  "                                                                                                                                                                                            ",
  "                                                                                                                                                                                            ",
  "                                                                                                                                                                                            ",
  "                                                                                                                            --      --       ^^^^                                           ",
  "                                                                                                               --          ^^^^    ^^^^      xxxx         ^                                 ",
  "                                                                                                           -      xxxx  xxxxxxxxxxxxxxx               xxxxx                                 ",
  "                                                        :                                              -                xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx ^              ^                ",
  "                                                       <x                                          -                    xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx xxxxxxxxx  ----x        ^       ",
  "                                    i                  <x                                      -                        xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx                   ------x       ",
  "                               i    x                                            ^         -                            xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx                           ----- ",
  "                          i    x    x                             ^        xxxxxxxxxxxxx                                xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx                                 ",
  "_________^______^^_______^x____x____x_________^^___xxxxxxxx__xxxxxxxxxxx___xxxxxxxxxxxxx________________________________xxxxxxxxxxxxxxx_xxxxxxxxxxxxxxxxxxx_________________________________",
);

Loader*
Loader_ForTestCourse() {
  return &loader;
}

static bool
Loader_GetSymbolAt(
    Loader *loader,
    int x, int y,
    char *symbol)
{
  const Vector *size = &loader->size;
  const int limit = loader->limit;

  if (x >= size->x || y >= size->y || x >= limit) {
    return false;
  }

  int index = y * size->x + x;
  *symbol = loader->layout[index];

  if (*symbol == '\0') {
    return false;
  }

  Vector *cursor = &loader->cursor;

  cursor->x = x;
  cursor->y = y;

  return true;
}

static inline Vector
Loader_GetCursorPosition(Loader *loader) {
  Vector *cursor = &loader->cursor;
  const Vector *size = &loader->size;

  Vector position = {
    .x = cursor->x * 2,
    .y = cursor->y * 2,
  };

  Chunk *chunk = loader->chunk;
  const Bounds *bounds = Chunk_GetBounds(chunk);

  int dy = (bounds->size.y >> 2) - size->y*2;
  // allign at bottom of chunk bounds
  position.y += dy;

  return position;
}

static inline bool
Loader_IsCursorOccupied(Loader *loader) {
  Vector position = Loader_GetCursorPosition(loader);

  int x = position.x * 8 + 8;
  int y = position.y * 8 + 8;

  Bounds hitbox = Bounds_Of(x, y, 8, 8);

  Unit unit = Unit_Of(&hitbox, NULL);
  Hit hit = Chunk_CheckHits(loader->chunk, &unit, NULL);

  return Hit_IsHit(&hit);
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

  return Loader_GetSymbolAt(loader, x, y, symbol)
     && !Loader_IsCursorOccupied(loader);
}

static inline void
Loader_AddObjectToChunk(
    Loader *loader,
    Object *template)
{
  Vector position = Loader_GetCursorPosition(loader);
  Object_Move(template, &position);

  Chunk *chunk = loader->chunk;

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
Loader_AddBoxWithPole(Loader *loader) {
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
    Loader_AddObjectToChunk(loader, &object);
  }
}

static void
Loader_AddBox(Loader *loader) {
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
    Loader_AddObjectToChunk(loader, &object);
  }
}

static void
Loader_AddDisk(Loader *loader) {
  Object object = {0};
  if (Object_CreateDisk(&object)) {
    Loader_AddObjectToChunk(loader, &object);
  }
}

static void
Loader_AddSpike(
    Loader *loader,
    Direction direction)
{
  Object object = {0};
  if (Object_CreateSpike(&object, direction)) {
    Loader_AddObjectToChunk(loader, &object);
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
  loader->limit = upper.x;
  loader->chunk = chunk;

  for (int x = lower.x; x < upper.x; x++) {
    for (int y = lower.y; y < upper.y; y++) {
      char symbol;
      if (x == 14 && y == 3) {
        symbol = 'a';
      }
      if (!Loader_GetSymbolAt(loader, x, y, &symbol)) {
        break; // continue with next x
      }

      switch (symbol) {
      case 'i':
        Loader_AddBoxWithPole(loader);
        break;
      case 'x':
        Loader_AddBox(loader);
        break;
      case '-':
        Loader_AddDisk(loader);
        break;
      case '^':
        Loader_AddSpike(loader, DIRECTION_UP);
        break;
      case '<':
        Loader_AddSpike(loader, DIRECTION_LEFT);
        break;
      case '>':
        Loader_AddSpike(loader, DIRECTION_RIGHT);
        break;
      }
    }
  }
}
