
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
  const int size = level->size.x;
  const int limit = level->limit;

  const int cursor = offset + length;
  if (cursor > size || (limit && cursor > limit)) {
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

static inline bool
Binv1Level_ResetCursor(Level *level) {
  level->chunk = NULL;
  level->cursor.x = 0;
}

static bool
Binv1Level_SetCursorTo(
    Level *level,
    Chunk *chunk)
{
  level->chunk = chunk;
  level->cursor.x = 0;

  // skip header
  int header = 0;
  Binv1Level_ReadInt8(level, &header);
  Binv1Level_AdvanceCursor(level, header);

  int index = chunk->index;
  while (index-- > 0) {
    int count = 0;
    if (!Binv1Level_ReadInt8(level, &count)) {
      // no more data to read
      return false;
    }

    int offset = count * 55;
    if (!Binv1Level_AdvanceCursor(level, offset)) {
      // no more data to read
      return false;
    }
  }

  return true;
}

static int
Binv1Level_GetMetaData(
    Level *level,
    int key)
{
  Binv1Level_ResetCursor(level);

  int header = 0;
  Binv1Level_ReadInt8(level, &header);

  while (true) {
    int tag;
    if (!Binv1Level_ReadInt8(level, &tag)) {
      // no more data to read
      return -1;
    }

    int length;
    if (!Binv1Level_ReadInt8(level, &length)) {
      // no more data to read
      return -1;
    }

    if (tag == key) {
      return length;
    }

    Binv1Level_AdvanceCursor(level, length);
  }
}

void
Binv1Level_GetName(
    Level *level,
    char *name)
{
  int length = Binv1Level_GetMetaData(level, 'n');
  while (length-- > 0) {
    int symbol;
    Binv1Level_ReadInt8(level, &symbol);

    *(name++) = (char) (symbol & 0xFF); // FIXME improve this
  }

  *name = '\0';
}

int
Binv1Level_GetChunkCount(Level *level) {
  int index = 1;
  Chunk chunk = {0};

  do {
    Chunk_AssignIndex(&chunk, index);

    if (!Binv1Level_SetCursorTo(level, &chunk)) {
      return index - 1;
    }

    index++;
  } while (true);
}

static void
Binv1Level_ReadObject(
    Level *level,
    Object *object)
{
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

  int solid = 0, deadly = 0;
  Binv1Level_ReadInt8(level, &solid);
  Binv1Level_ReadInt8(level, &deadly);

  object->type = type;
  object->solid = solid == 1;
  object->deadly = deadly == 1;

  Binv1Level_ReadValue(level, &object->properties, sizeof(object->properties));
}

bool
Binv1Level_GetChunk(
    Level *level,
    Chunk *chunk)
{
  if (!Binv1Level_SetCursorTo(level, chunk)) {
    return false;
  }

  int count = 0;
  Binv1Level_ReadInt8(level, &count);

  while (count-- > 0) {
    Object *object = Chunk_AllocateObject(chunk);

    if (object != NULL) {
      Binv1Level_ReadObject(level, object);
      Chunk_AddObject(chunk, object);
    }
  }

  return true;
}

static void
Binv1Level_WriteMetaData(
    Level *level,
    int key,
    char *value)
{
  Binv1Level_ResetCursor(level);

  int length = 0;
  while (value[length++] != '\0');

  int header = 0;
  Binv1Level_ReadInt8(level, &header);
  Binv1Level_AdvanceCursor(level, header);

  Binv1Level_WriteInt8(level, key);
  Binv1Level_WriteInt8(level, length);

  for (int i = 0; i < length; i++) {
    Binv1Level_WriteInt8(level, value[i]);
  }

  header += length + 2;

  Binv1Level_ResetCursor(level);
  Binv1Level_WriteInt8(level, header);
}

void
Binv1Level_SetName(
    Level *level,
    char *name)
{
  Binv1Level_WriteMetaData(level, 'n', name);
}

static void
Binv1Level_WriteObject(
    Level *level,
    Object *object)
{
  Binv1Level_WriteInt8(level, object->type);

  Binv1Level_WriteInt16(level, object->hitbox.center.x);
  Binv1Level_WriteInt16(level, object->hitbox.center.y);
  Binv1Level_WriteInt16(level, object->hitbox.size.x);
  Binv1Level_WriteInt16(level, object->hitbox.size.y);

  Binv1Level_WriteInt16(level, object->viewbox.center.x);
  Binv1Level_WriteInt16(level, object->viewbox.center.y);
  Binv1Level_WriteInt16(level, object->viewbox.size.x);
  Binv1Level_WriteInt16(level, object->viewbox.size.y);

  Binv1Level_WriteInt8(level, object->solid ? 1 : 0);
  Binv1Level_WriteInt8(level, object->deadly ? 1 : 0);

  Binv1Level_WriteValue(level, object->properties, sizeof(object->properties));
}

bool
Binv1Level_AddChunk(
    Level *level,
    Chunk *chunk)
{
  if (!Binv1Level_SetCursorTo(level, chunk)) {
    return false;
  }
  Binv1Level_WriteInt8(level, chunk->count);

  for (int i = 0; i < chunk->count; i++) {
    Object *object = &chunk->objects[i];
    Binv1Level_WriteObject(level, object);
  }

  return true;
}
