
#include <game/collection.h>

static const Collection *collection = Collection_DefineNew(const, 300 * 1024);

const Collection*
Collection_GetInstance() {
  return collection;
}

int
Collection_GetLevelCount(const Collection *collection) {
  return collection->count;
}

static const unsigned char*
Collection_GetReadBuffer(const Collection *collection) {
  const unsigned char *buffer = (const unsigned char *) collection;
  const int header = sizeof(Collection);
  return &buffer[header];
}

static unsigned char*
Collection_GetWriteBuffer(Collection *collection) {
  unsigned char *buffer = (unsigned char *) collection;
  const int header = sizeof(Collection);
  return &buffer[header];
}

Binv1Level*
Collection_GetLevelByIndex(
    const Collection *collection,
    int index)
{
  static Buffer buffer;
  static Binv1Level level;

  if (index < collection->count && index >= 0) {
    int length = collection->allocations[index];
    int offset = 0;

    for (int i = 0; i < index; i++) {
      offset += collection->allocations[i];
    }

    // FIXME this is const (Buffer_Wrap requires non const)
    /*const*/ unsigned char *data = (unsigned char *) Collection_GetReadBuffer(collection);

    DataSource *source = Buffer_From(&buffer, &data[offset], length);
    Binv1Level_From(&level, source);

    return &level;
  }

  return NULL;
}

static bool
Collection_CanAllocate(
    const Collection *collection,
    int offset,
    int length)
{
  const int header = sizeof(Collection);
  const int size = collection->length;
  const int limit = size - header;

  return offset + length <= limit;
}

bool
Collection_AddLevel(
    Collection *collection,
    const Binv1Level *level)
{
  int index = collection->count;
  if (index >= length(collection->allocations)) {
    return false;
  }

  int offset = 0;
  for (int i = 0; i < collection->count; i++) {
    offset += collection->allocations[i];
  }

  int length = level->size;
  if (!Collection_CanAllocate(collection, offset, length)) {
    return false;
  }

  unsigned char *data = Collection_GetWriteBuffer(collection);

  DataSource *target = Buffer_From(&(Buffer) {0}, &data[offset], length);
  Writer *writer = DataSource_AsWriter(target);

  DataSource *source = level->source;
  Reader *reader = DataSource_AsReader(source);

  for (int i = 0; i < length; i++) {
    int byte = Reader_Read(reader);
    if (byte < 0) {
      return false;
    }

    if (!Writer_Write(writer, byte)) {
      return false;
    }
  }

  collection->allocations[index] = length;
  collection->count++;

  return true;
}

bool
Collection_FindSignature(const Reader *reader) {
  const Collection *collection = Collection_DefineWithUsableSpace(0);

  const int length = sizeof(collection->signature);
  int index = 0;

  do {
    int byte = Reader_Read(reader);
    if (byte < 0) {
      return false;
    }

    if (byte == collection->signature[index]) {
      index++;
    } else {
      index = 0;
    }
  } while (index != length);

  return true;
}

bool
Collection_ReadFrom(
    volatile Collection *collection,
    const Reader *reader)
{
  int index = sizeof(collection->signature);

  if (!Collection_FindSignature(reader)) {
    return false;
  }

  unsigned char *buffer = (unsigned char *) collection;
  const int header = sizeof(*collection);
  const int length = collection->length;

  while (index < header) {
    int byte = Reader_Read(reader);
    if (byte < 0) {
      return false;
    }

    buffer[index++] = byte;
  }

  if (collection->length > length) {
    return false;
  }

  const int limit = collection->length;
  do {
    int byte = Reader_Read(reader);
    if (byte < 0) {
      return false;
    }

    buffer[index++] = byte;
  } while (index != limit);

  return true;
}

bool
Collection_WriteTo(
    const Collection *collection,
    const Writer *writer)
{
  const unsigned char *buffer = (const unsigned char *) collection;
  const int length = collection->length;

  int index = sizeof(collection->signature);
  while (index < length) {
    if (!Writer_Write(writer, buffer[index++])) {
      return false;
    }
  }

  return true;
}
