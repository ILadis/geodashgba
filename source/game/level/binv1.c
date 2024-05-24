
#include <game/level.h>

Level*
Binv1Level_From(
    Binv1Level *level,
    DataSource *source)
{
  level->source = source;
  level->base.self = level;

  int  Binv1Level_GetId(void *self);
  void Binv1Level_SetId(void *self, int id);
  void Binv1Level_GetName(void *self, char *name);
  void Binv1Level_SetName(void *self, char *name);
  int  Binv1Level_GetChunkCount(void *self);
  bool Binv1Level_GetChunk(void *self, Chunk *chunk);
  bool Binv1Level_AddChunk(void *self, Chunk *chunk);
  Object* Binv1Level_NextObject(void *self, Chunk *chunk);

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

static inline bool
Binv1Level_IsLittleEndian() {
  const short int value = 1;
  const char *endian = (char *) &value;

  return *endian == 1;
}

static bool
Binv1Level_ReadValue(
    Binv1Level *level,
    void *value,
    int length)
{
  Reader *reader = DataSource_AsReader(level->source);
  unsigned char *values = value;

  if (Binv1Level_IsLittleEndian()) {
    for (int i = 0; i < length; i++) {
      int byte = Reader_Read(reader);
      if (byte < 0) {
        return false;
      }

      values[i] = byte;
    }
  }

  return true;
  // big endian is unsupported
}

// TODO use equivalent reader functions instead (see io.h)

static inline bool
Binv1Level_ReadInt8(Binv1Level *level, int *value) {
  return Binv1Level_ReadValue(level, value, 1);
}

static inline bool
Binv1Level_ReadUInt8(Binv1Level *level, unsigned int *value) {
  return Binv1Level_ReadValue(level, value, 1);
}

static inline bool
Binv1Level_ReadInt16(Binv1Level *level, int *value) {
  return Binv1Level_ReadValue(level, value, 2);
}

static bool
Binv1Level_WriteValue(
    Binv1Level *level,
    void *value,
    int length)
{
  Writer *writer = DataSource_AsWriter(level->source);
  unsigned char *values = value;

  if (Binv1Level_IsLittleEndian()) {
    for (int i = 0; i < length; i++) {
      if (!Writer_Write(writer, values[i])) {
        return false;
      }
    }
  }

  return true;
  // big endian is unsupported
}

// TODO use equivalent writer functions instead (see io.h)

static inline bool
Binv1Level_WriteInt8(Binv1Level *level, int value) {
  return Binv1Level_WriteValue(level, &value, 1);
}

static inline bool
Binv1Level_WriteUInt8(Binv1Level *level, unsigned int value) {
  return Binv1Level_WriteValue(level, &value, 1);
}

static inline bool
Binv1Level_WriteInt16(Binv1Level *level, int value) {
  return Binv1Level_WriteValue(level, &value, 2);
}

static bool
Binv1Level_SetCursorTo(
    Binv1Level *level,
    Chunk *chunk)
{
  Reader *reader = DataSource_AsReader(level->source);
  Reader_SeekTo(reader, 0);

  // skip header
  unsigned int header = 0;
  Binv1Level_ReadUInt8(level, &header);

  unsigned int offset = header + 1;
  Reader_SeekTo(reader, offset);

  int index = chunk->index;
  while (index-- > 0) {
    int count = 0;
    if (!Binv1Level_ReadInt8(level, &count)) {
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

static unsigned int
Binv1Level_GetMetaData(
    Binv1Level *level,
    int key)
{
  Reader *reader = DataSource_AsReader(level->source);
  Reader_SeekTo(reader, 0);

  // TODO limit further reading to size of header
  unsigned int header = 0;
  Binv1Level_ReadUInt8(level, &header);

  unsigned int offset = 1;
  while (true) {
    int tag = 0;
    if (!Binv1Level_ReadInt8(level, &tag)) {
      // no more data to read
      return -1;
    }

    unsigned int length = 0;
    if (!Binv1Level_ReadUInt8(level, &length)) {
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

int
Binv1Level_GetId(void *self) {
  Binv1Level *level = self;

  int id = 0;
  unsigned int length = Binv1Level_GetMetaData(level, 'i');
  while (length > 0) {
    int byte = 0;
    Binv1Level_ReadInt8(level, &byte);

    id = (id << 8) | (byte & 0xFF);
    length--;
  }

  return id;
}

void
Binv1Level_GetName(
    void *self,
    char *name)
{
  Binv1Level *level = self;

  unsigned int length = Binv1Level_GetMetaData(level, 'n');
  while (length > 0) {
    Binv1Level_ReadInt8(level, (int*) name++);
    length--;
  }
  *name = '\0';
}

int
Binv1Level_GetChunkCount(void *self) {
  Binv1Level *level = self;
  Chunk chunk = {0};

  int index = 1;
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
    Binv1Level *level,
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
    void *self,
    Chunk *chunk)
{
  Binv1Level *level = self;
  if (!Binv1Level_SetCursorTo(level, chunk)) {
    return false;
  }

  unsigned int count = 0;
  Binv1Level_ReadUInt8(level, &count);

  while (count-- > 0) {
    Object *object = Chunk_AllocateObject(chunk);

    if (object != NULL) {
      Binv1Level_ReadObject(level, object);
      Chunk_AddObject(chunk, object);
    }
  }

  return true;
}

Object*
Binv1Level_NextObject(
    void *self,
    Chunk *chunk)
{
  Binv1Level *level = self;
  if (!Binv1Level_SetCursorTo(level, chunk)) {
    return NULL;
  }

  unsigned int count = 0;
  Binv1Level_ReadUInt8(level, &count);

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
    int key,
    char *value,
    unsigned int length)
{
  Reader *reader = DataSource_AsReader(level->source);
  Reader_SeekTo(reader, 0);

  unsigned int header = 0;
  Binv1Level_ReadUInt8(level, &header);

  unsigned int offset = header + 1;
  Reader_SeekTo(reader, offset);

  Binv1Level_WriteInt8(level, key);
  Binv1Level_WriteUInt8(level, length);

  for (unsigned int i = 0; i < length; i++) {
    Binv1Level_WriteInt8(level, value[i]);
  }

  header += length + 2;

  Reader_SeekTo(reader, 0);
  Binv1Level_WriteUInt8(level, header);
}

void
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

void
Binv1Level_SetName(
    void *self,
    char *name)
{
  Binv1Level *level = self;

  int length = 0;
  while (name[length] != '\0') length++;

  Binv1Level_WriteMetaData(level, 'n', name, length);
}

static void
Binv1Level_WriteObject(
    Binv1Level *level,
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
    void *self,
    Chunk *chunk)
{
  Binv1Level *level = self;
  if (!Binv1Level_SetCursorTo(level, chunk)) {
    return false;
  }
  Binv1Level_WriteUInt8(level, chunk->count);

  for (unsigned int i = 0; i < chunk->count; i++) {
    Object *object = &chunk->objects[i];
    Binv1Level_WriteObject(level, object);
  }

  return true;
}
