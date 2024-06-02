
#include <game/level.h>

static inline bool
Binv1Level_SetCursorToIndex(
    Binv1Level *level,
    unsigned int index)
{
  Reader *reader = DataSource_AsReader(level->source);
  Reader_SeekTo(reader, 0);

  // skip header
  unsigned char header = 0;
  Reader_ReadUInt8(reader, &header);

  unsigned int offset = header + 1;
  Reader_SeekTo(reader, offset);

  while (index-- > 0) {
    char count = 0;
    if (!Reader_ReadInt8(reader, &count)) {
      // no more data to read
      return false;
    }

    offset += count * 55 + 1;
    if (!Reader_SeekTo(reader, offset)) {
      // no more data to read
      return false;
    }
  }

  return true;
}

static bool
Binv1Level_SetCursorTo(
    Binv1Level *level,
    Chunk *chunk)
{
  return Binv1Level_SetCursorToIndex(level, chunk->index);
}

static unsigned int
Binv1Level_GetMetaData(
    Binv1Level *level,
    int key)
{
  Reader *reader = DataSource_AsReader(level->source);
  Reader_SeekTo(reader, 0);

  // TODO limit further reading to size of header
  unsigned char header = 0;
  Reader_ReadUInt8(reader, &header);

  unsigned int offset = 1;
  while (true) {
    char tag = 0;
    if (!Reader_ReadInt8(reader, &tag)) {
      // no more data to read
      return -1;
    }

    unsigned char length = 0;
    if (!Reader_ReadUInt8(reader, &length)) {
      // no more data to read
      return -1;
    }

    if (tag == key) {
      return length;
    }

    offset += 2 + length;
    if (!Reader_SeekTo(reader, offset)) {
      // no more data to read
      return false;
    }
  }
}

static int
Binv1Level_GetId(void *self) {
  Binv1Level *level = self;
  Reader *reader = DataSource_AsReader(level->source);

  int id = 0;
  unsigned int length = Binv1Level_GetMetaData(level, 'i');
  while (length > 0) {
    char byte = 0;
    Reader_ReadInt8(reader, &byte);

    id = (id << 8) | (byte & 0xFF);
    length--;
  }

  return id;
}

static void
Binv1Level_GetName(
    void *self,
    char *name)
{
  Binv1Level *level = self;
  Reader *reader = DataSource_AsReader(level->source);

  unsigned int length = Binv1Level_GetMetaData(level, 'n');
  while (length > 0) {
    Reader_ReadInt8(reader, name++);
    length--;
  }
  *name = '\0';
}

static int
Binv1Level_GetChunkCount(void *self) {
  Binv1Level *level = self;

  unsigned int index = 1;
  do {
    if (!Binv1Level_SetCursorToIndex(level, index)) {
      return index - 1;
    }

    index++;
  } while (true);
}

static inline void
Binv1Level_ReadVector(
    Binv1Level *level,
    Vector *vector)
{
  Reader *reader = DataSource_AsReader(level->source);

  short x = 0, y = 0;

  Reader_ReadInt16(reader, &x);
  Reader_ReadInt16(reader, &y);

  vector->x = x;
  vector->y = y;
}

static void
Binv1Level_ReadObject(
    Binv1Level *level,
    Object *object)
{
  Reader *reader = DataSource_AsReader(level->source);

  char type = 0;
  Reader_ReadInt8(reader, &type);

  Binv1Level_ReadVector(level, &object->hitbox.center);
  Binv1Level_ReadVector(level, &object->hitbox.size);

  Binv1Level_ReadVector(level, &object->viewbox.center);
  Binv1Level_ReadVector(level, &object->viewbox.size);

  char solid = 0, deadly = 0;
  Reader_ReadInt8(reader, &solid);
  Reader_ReadInt8(reader, &deadly);

  object->type = type;
  object->solid = solid == 1;
  object->deadly = deadly == 1;

  Reader_Read(reader, &object->properties, sizeof(object->properties));
}

static bool
Binv1Level_GetChunk(
    void *self,
    Chunk *chunk)
{
  Binv1Level *level = self;
  Reader *reader = DataSource_AsReader(level->source);

  if (!Binv1Level_SetCursorTo(level, chunk)) {
    return false;
  }

  unsigned char count = 0;
  Reader_ReadUInt8(reader, &count);

  while (count-- > 0) {
    Object *object = Chunk_AllocateObject(chunk);

    if (object != NULL) {
      Binv1Level_ReadObject(level, object);
      Chunk_AddObject(chunk, object);
    }
  }

  return true;
}

static Object*
Binv1Level_NextObject(
    void *self,
    Chunk *chunk)
{
  Binv1Level *level = self;
  Reader *reader = DataSource_AsReader(level->source);

  if (!Binv1Level_SetCursorTo(level, chunk)) {
    return NULL;
  }

  unsigned char count = 0;
  Reader_ReadUInt8(reader, &count);

  if (chunk->count < count) {
    Object *object = Chunk_AllocateObject(chunk);
    if (object != NULL) {
      for (unsigned int i = 0; i < chunk->count; i++) {
        Binv1Level_ReadObject(level, object);
      }
      Chunk_AddObject(chunk, object);
    }

    return object;
  }

  return NULL;
}

static void
Binv1Level_WriteMetaData(
    Binv1Level *level,
    char key, char *value,
    unsigned char length)
{
  Reader *reader = DataSource_AsReader(level->source);
  Reader_SeekTo(reader, 0);

  unsigned char header = 0;
  Reader_ReadUInt8(reader, &header);

  unsigned int offset = header + 1;

  Writer *writer = DataSource_AsWriter(level->source);
  Writer_SeekTo(writer, offset);
  Writer_WriteInt8(writer, key);
  Writer_WriteUInt8(writer, length);

  for (unsigned int i = 0; i < length; i++) {
    Writer_WriteInt8(writer, value[i]);
  }

  header += length + 2;

  Writer_SeekTo(writer, 0);
  Writer_WriteUInt8(writer, header);
}

static void
Binv1Level_SetId(
    void *self,
    int id)
{
  Binv1Level *level = self;

  char data[] = {
    (char) ((id >> 24) & 0xFF),
    (char) ((id >> 16) & 0xFF),
    (char) ((id >>  8) & 0xFF),
    (char) ((id >>  0) & 0xFF),
  };

  Binv1Level_WriteMetaData(level, 'i', data, length(data));
}

static void
Binv1Level_SetName(
    void *self,
    char *name)
{
  Binv1Level *level = self;

  unsigned char length = 0;
  while (name[length] != '\0') length++;

  Binv1Level_WriteMetaData(level, 'n', name, length);
}

static inline void
Binv1Level_WriteVector(
    Binv1Level *level,
    Vector *vector)
{
  Writer *writer = DataSource_AsWriter(level->source);

  short x = (short) vector->x, y = (short) vector->y;

  Writer_WriteInt16(writer, x);
  Writer_WriteInt16(writer, y);
}

static void
Binv1Level_WriteObject(
    Binv1Level *level,
    Object *object)
{
  Writer *writer = DataSource_AsWriter(level->source);

  char type = (char) object->type;
  Writer_WriteInt8(writer, type);

  Binv1Level_WriteVector(level, &object->hitbox.center);
  Binv1Level_WriteVector(level, &object->hitbox.size);

  Binv1Level_WriteVector(level, &object->viewbox.center);
  Binv1Level_WriteVector(level, &object->viewbox.size);

  Writer_WriteInt8(writer, object->solid ? 1 : 0);
  Writer_WriteInt8(writer, object->deadly ? 1 : 0);

  Writer_Write(writer, object->properties, sizeof(object->properties));
}

static bool
Binv1Level_AddChunk(
    void *self,
    Chunk *chunk)
{
  Binv1Level *level = self;
  Writer *writer = DataSource_AsWriter(level->source);

  if (!Binv1Level_SetCursorTo(level, chunk)) {
    return false;
  }
  Writer_WriteUInt8(writer, chunk->count);

  for (unsigned int i = 0; i < chunk->count; i++) {
    Object *object = &chunk->objects[i];
    Binv1Level_WriteObject(level, object);
  }

  return true;
}

Level*
Binv1Level_From(
    Binv1Level *level,
    DataSource *source)
{
  level->source = source;
  level->base.self = level;

  level->base.GetId = Binv1Level_GetId;
  level->base.SetId = Binv1Level_SetId;
  level->base.GetName = Binv1Level_GetName;
  level->base.SetName = Binv1Level_SetName;
  level->base.GetChunkCount = Binv1Level_GetChunkCount;
  level->base.GetChunk = Binv1Level_GetChunk;
  level->base.AddChunk = Binv1Level_AddChunk;
  level->base.NextObject = Binv1Level_NextObject;

  return &level->base;
}
