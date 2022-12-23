
#include <game/level.h>

static inline bool
Binv1Level_IsLittleEndian() {
  const short int value = 1;
  const char *endian = (char *) &value;

  return *endian == 1;
}

static inline bool
Binv1Level_AdvanceCursor(
    Level *level,
    int length)
{
  const int offset = level->cursor.x;
  const int limit = level->size.x;

  const int cursor = offset + length;
  if (cursor > limit) {
    return false;
  }

  level->cursor.x = cursor;

  return true;
}

static bool
Binv1Level_ReadValue(
    Level *level,
    void *value,
    int length)
{
  const int offset = level->cursor.x;

  if (!Binv1Level_AdvanceCursor(level, length)) {
    return false;
  }

  unsigned char *buffer = level->buffer.write;
  unsigned char *values = value;

  if (Binv1Level_IsLittleEndian()) {
    for (int i = 0; i < length; i++) {
      values[i] = buffer[offset + i];
    }
  }

  return true;
  // big endian is unsupported
}

static inline bool
Binv1Level_ReadInt8(Level *level, int *value) {
  return Binv1Level_ReadValue(level, value, 1);
}

static inline bool
Binv1Level_ReadInt16(Level *level, int *value) {
  return Binv1Level_ReadValue(level, value, 2);
}

static bool
Binv1Level_WriteValue(
    Level *level,
    void *value,
    int length)
{
  const int offset = level->cursor.x;

  if (!Binv1Level_AdvanceCursor(level, length)) {
    return false;
  }

  unsigned char *buffer = level->buffer.write;
  unsigned char *values = value;

  if (Binv1Level_IsLittleEndian()) {
    for (int i = 0; i < length; i++) {
      buffer[offset + i] = values[i];
    }
  }

  return true;
  // big endian is unsupported
}

static inline void
Binv1Level_WriteInt8(Level *level, int value) {
  Binv1Level_WriteValue(level, &value, 1);
}

static inline void
Binv1Level_WriteInt16(Level *level, int value) {
  Binv1Level_WriteValue(level, &value, 2);
}

static bool
Binv1Level_ResetCursor(
    Level *level,
    Chunk *chunk)
{
  level->chunk = chunk;
  level->cursor.x = 0;

  int index = chunk->index;

  while (index-- > 0) {
    int count = 0;
    if (!Binv1Level_ReadInt8(level, &count)) {
      // no more data to read
      return false;
    }

    int offset = count * 51;
    if (!Binv1Level_AdvanceCursor(level, offset)) {
      // no more data to read
      return false;
    }
  }

  return true;
}

void
Binv1Level_GetChunk(
    Level *level,
    Chunk *chunk)
{
  Binv1Level_ResetCursor(level, chunk);

  int count = 0;
  Binv1Level_ReadInt8(level, &count);

  while (count-- > 0) {
    Object *object = Chunk_AllocateObject(chunk);

    if (object != NULL) {
      Chunk_AddObject(chunk, object);

      int type = 0;
      Binv1Level_ReadInt8(level, &type);

      Binv1Level_ReadInt16(level, &object->hitbox.center.x);
      Binv1Level_ReadInt16(level, &object->hitbox.center.y);
      Binv1Level_ReadInt16(level, &object->hitbox.size.x);
      Binv1Level_ReadInt16(level, &object->hitbox.size.y);

      Binv1Level_ReadInt16(level, &object->viewbox.center.x);
      Binv1Level_ReadInt16(level, &object->viewbox.center.y);
      Binv1Level_ReadInt16(level, &object->viewbox.size.x);
      Binv1Level_ReadInt16(level, &object->viewbox.size.y);

      int solid = 0, deadly;
      Binv1Level_ReadInt8(level, &solid);
      Binv1Level_ReadInt8(level, &deadly);

      object->type = (enum Type) type;
      object->solid = solid == 1;
      object->deadly = deadly == 1;

      Binv1Level_ReadValue(level, &object->properties, sizeof(object->properties));
    }
  }
}

void
Binv1Level_AddChunk(
    Level *level,
    Chunk *chunk)
{
  Binv1Level_ResetCursor(level, chunk);
  Binv1Level_WriteInt8(level, chunk->count);

  for (int i = 0; i < chunk->count; i++) {
    Object *object = &chunk->objects[i];

    Binv1Level_WriteInt8(level, object->type);

    Binv1Level_WriteInt16(level, object->hitbox.center.x);
    Binv1Level_WriteInt16(level, object->hitbox.center.y);
    Binv1Level_WriteInt16(level, object->hitbox.size.x);
    Binv1Level_WriteInt16(level, object->hitbox.size.y);

    Binv1Level_WriteInt16(level, object->viewbox.center.x);
    Binv1Level_WriteInt16(level, object->viewbox.center.y);
    Binv1Level_WriteInt16(level, object->viewbox.size.x);
    Binv1Level_WriteInt16(level, object->viewbox.size.y);

    Binv1Level_WriteInt8(level, object->solid);
    Binv1Level_WriteInt8(level, object->deadly);

    Binv1Level_WriteValue(level, object->properties, sizeof(object->properties));
  }
}
