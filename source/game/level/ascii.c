
#include <game/level.h>

static bool
AsciiLevel_GetSymbolAt(
    Level *level,
    int x, int y,
    char *symbol)
{
  const Vector *size = &level->size;
  const int limit = level->limit;

  if (x >= size->x || y >= size->y || (limit && x >= limit)) {
    return false;
  }

  const char *buffer = level->buffer.read;
  int index = y * size->x + x;

  const char c = *symbol = buffer[index];
  if (c == '\0' || c == '\n') {
    return false;
  }

  Vector *cursor = &level->cursor;

  cursor->x = x;
  cursor->y = y;

  return true;
}

static Vector
AsciiLevel_GetCursorPosition(Level *level) {
  Vector *cursor = &level->cursor;
  const Vector *size = &level->size;

  Vector position = {
    .x = cursor->x * 2,
    .y = cursor->y * 2,
  };

  Chunk *chunk = level->chunk;
  const Bounds *bounds = Chunk_GetBounds(chunk);

  int dy = (bounds->size.y >> 2) - size->y*2;
  // allign at bottom of chunk bounds
  position.y += dy;

  return position;
}

static bool
AsciiLevel_IsCursorOccupied(Level *level) {
  Vector position = AsciiLevel_GetCursorPosition(level);

  int x = position.x * 8 + 8;
  int y = position.y * 8 + 8;

  Bounds hitbox = Bounds_Of(x, y, 8, 8);

  Unit unit = Unit_Of(&hitbox, NULL);
  return Chunk_CheckHits(level->chunk, &unit, NULL);
}

static bool
AsciiLevel_NextSymbol(
    Level *level,
    Direction direction,
    char *symbol)
{
  const Vector *delta = Vector_FromDirection(direction);
  Vector *cursor = &level->cursor;

  int x = cursor->x + delta->x;
  int y = cursor->y + delta->y;

  return AsciiLevel_GetSymbolAt(level, x, y, symbol)
     && !AsciiLevel_IsCursorOccupied(level);
}

static int
AsciiLevel_CountConsecutiveSymbols(
    Level *level,
    Direction directon,
    char symbol)
{
  Vector cursor = level->cursor;
  int count = 0;

  char next;
  while (AsciiLevel_NextSymbol(level, DIRECTION_RIGHT, &next)) {
    if (next != symbol) break;
    else count++;
  }

  level->cursor = cursor;

  return count;
}

static inline void
AsciiLevel_AddObjectToChunk(
    Level *level,
    Object *template)
{
  Vector position = AsciiLevel_GetCursorPosition(level);
  Object_Move(template, &position);

  Chunk *chunk = level->chunk;

  Unit unit = Unit_Of(&template->hitbox, template);
  
  if (!Chunk_CheckHits(chunk, &unit, NULL)) {
    Object *object = Chunk_AllocateObject(chunk);
    if (object != NULL) {
      Object_AssignFrom(object, template);
      Chunk_AddObject(chunk, object);
    }
  }
}

static void
AsciiLevel_AddBoxWithPole(Level *level) {
  Object object = {0};
  Vector offset = Vector_Of(0, -1);

  int height = AsciiLevel_CountConsecutiveSymbols(level, DIRECTION_DOWN, 'x');

  if (Object_CreateBoxWithPole(&object, height)) {
    Object_Move(&object, &offset);
    AsciiLevel_AddObjectToChunk(level, &object);
  }
}

static void
AsciiLevel_AddBox(Level *level) {
  Vector cursor = level->cursor;
  int width = 1, height = 1;

  char symbol;
  while (AsciiLevel_NextSymbol(level, DIRECTION_RIGHT, &symbol)) {
    if (symbol != 'x') break;
    else width++;
  }

  level->cursor = cursor;

  while (AsciiLevel_NextSymbol(level, DIRECTION_DOWN, &symbol)) {
    if (symbol != 'x') break;

    int length = 1;
    while (AsciiLevel_NextSymbol(level, DIRECTION_RIGHT, &symbol)) {
      if (symbol != 'x') break;
      else length++;
    }

    if (length == width) height++;
    else break;

    level->cursor.x = cursor.x;
  }

  level->cursor = cursor;

  Object object = {0};
  if (Object_CreateBox(&object, width, height)) {
    AsciiLevel_AddObjectToChunk(level, &object);
  }
}

static void
AsciiLevel_AddPit(Level *level) {
  Object object = {0};
  Vector offset = Vector_Of(0, +1);

  int width = AsciiLevel_CountConsecutiveSymbols(level, DIRECTION_RIGHT, '.') + 1;

  if (Object_CreatePit(&object, width)) {
    Object_Move(&object, &offset);
    AsciiLevel_AddObjectToChunk(level, &object);
  }
}

static void
AsciiLevel_AddDisk(Level *level) {
  int width = AsciiLevel_CountConsecutiveSymbols(level, DIRECTION_RIGHT, '-') + 1;

  Object object = {0};
  if (Object_CreateDisk(&object, width)) {
    AsciiLevel_AddObjectToChunk(level, &object);
  }
}

static void
AsciiLevel_AddOffsetDisk(
    Level *level,
    Direction direction)
{
  int width = AsciiLevel_CountConsecutiveSymbols(level, DIRECTION_RIGHT, '~') + 1;

  Object object = {0};
  if (Object_CreateOffsetDisk(&object, direction, width)) {
    AsciiLevel_AddObjectToChunk(level, &object);
  }
}

static void
AsciiLevel_AddPad(Level *level) {
  Object object = {0};
  Vector offset = Vector_Of(0, +1);

  if (Object_CreatePad(&object)) {
    Object_Move(&object, &offset);
    AsciiLevel_AddObjectToChunk(level, &object);
  }
}

static void
AsciiLevel_AddSpike(
    Level *level,
    Direction direction)
{
  Object object = {0};
  if (Object_CreateSpike(&object, direction)) {
    AsciiLevel_AddObjectToChunk(level, &object);
  }
}

int
AsciiLevel_GetChunkCount(Level *level) {
  int count = 0;
  Chunk chunk = {0};

  level->limit = 0;

  do {
    Chunk_AssignIndex(&chunk, count);

    const Bounds *bounds = Chunk_GetBounds(&chunk);
    Vector lower = Bounds_Lower(bounds);
    Vector_Rshift(&lower, 4);

    char symbol;
    if (!AsciiLevel_GetSymbolAt(level, lower.x, lower.y, &symbol)) {
      return count;
    }

    count++;
  } while (true);
}

bool
AsciiLevel_GetChunk(
    Level *level,
    Chunk *chunk)
{
  const Bounds *bounds = Chunk_GetBounds(chunk);
  Vector lower = Bounds_Lower(bounds);
  Vector upper = Bounds_Upper(bounds);

  // convert to cursor coordinates
  Vector_Rshift(&lower, 4);
  Vector_Rshift(&upper, 4);

  // limit bounds of cursor to this chunk
  level->limit = upper.x;
  level->chunk = chunk;

  char symbol;
  if (!AsciiLevel_GetSymbolAt(level, lower.x, lower.y, &symbol)) {
    return false;
  }

  for (int x = lower.x; x < upper.x; x++) {
    for (int y = lower.y; y < upper.y; y++) {
      if (!AsciiLevel_GetSymbolAt(level, x, y, &symbol)) {
        break; // continue with next x
      }

      switch (symbol) {
      case 'i':
        AsciiLevel_AddBoxWithPole(level);
        break;
      case 'x':
        AsciiLevel_AddBox(level);
        break;
      case '.':
        AsciiLevel_AddPit(level);
        break;
      case '-':
        AsciiLevel_AddDisk(level);
        break;
      case '~':
        AsciiLevel_AddOffsetDisk(level, DIRECTION_UP);
        break;
      case 'T':
        AsciiLevel_AddPad(level);
        break;
      case '^':
        AsciiLevel_AddSpike(level, DIRECTION_UP);
        break;
      case 'v':
        AsciiLevel_AddSpike(level, DIRECTION_DOWN);
        break;
      case '<':
        AsciiLevel_AddSpike(level, DIRECTION_LEFT);
        break;
      case '>':
        AsciiLevel_AddSpike(level, DIRECTION_RIGHT);
        break;
      }
    }
  }

  return true;
}

bool
AsciiLevel_AddChunk(
    Level *level,
    Chunk *chunk)
{
  // currently unsupported
  return false;
}
