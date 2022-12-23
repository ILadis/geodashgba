
#include <game/level.h>

static inline bool
Bin1Level_IsLittleEndian() {
  const short int value = 1;
  const char *endian = (char *) &value;

  return *endian == 1;
}

static inline Prototype*
Bin1Level_GetPrototypeById(unsigned char id) {
  Object template = {0};

  if (Object_CreateDisk(&template) && template.proto->id == id) {
    return template.proto;
  }

/*
  if (Object_CreateBox(&template, 1, 1) && template.proto == object->proto) {
    return 'x';
  }

  if (Object_CreateBoxWithPole(&template, 1) && template.proto == object->proto) {
    return 'i';
  }

  if (Object_CreateSpike(&template, 1) && template.proto == object->proto) {
    return '^';
  }
*/

  return NULL;
}

static void
Binv1Level_WriteValue(
    Level *level,
    void *value,
    int size,
    int length)
{
  const int offset = level->cursor.x;

  unsigned char *buffer = level->buffer.write;
  unsigned char *values = value;

  level->cursor.x += length;

// TODO check for end of buffer
//if (buffer->position > buffer->limit) return;

  if (Bin1Level_IsLittleEndian()) {
    int j = 0;
    for (int i = 0; i < length; i++) {
      buffer[offset + i] = values[j++];
    }
  }

/* unsupported:
  else {
    int j = size;
    for (int i = 0; i < length; i++) {
      buffer[offset + i] = values[--j];
    }
  }
*/
}

void
Binv1Level_WriteInt8(Level *level, int value) {
  Binv1Level_WriteValue(level, &value, sizeof(int), 1);
}

void
Binv1Level_WriteInt16(Level *level, int value) {
  Binv1Level_WriteValue(level, &value, sizeof(int), 2);
}

void
Binv1Level_GetChunk(
    Level *level,
    Chunk *chunk)
{
  // currently unsupported
}

void
Binv1Level_AddChunk(
    Level *level,
    Chunk *chunk)
{
  Binv1Level_WriteInt8(level, chunk->count);

  for (int i = 0; i < chunk->count; i++) {
    Object *object = &chunk->objects[i];

    Binv1Level_WriteInt8(level, object->proto->id);

    Binv1Level_WriteInt16(level, object->hitbox.center.x);
    Binv1Level_WriteInt16(level, object->hitbox.center.y);
    Binv1Level_WriteInt16(level, object->hitbox.size.x);
    Binv1Level_WriteInt16(level, object->hitbox.size.y);
  }
}
