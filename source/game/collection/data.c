
#include <game/collection.h>

static const DataCollection *collection = DataCollection_DefineNew(const, 300 * 1024);

static int GetLevelCount() {
  int DataCollection_GetLevelCount(const DataCollection *collection);
  return DataCollection_GetLevelCount(collection);
}

static Binv1Level* GetLevelByIndex(int index) {
  Binv1Level* DataCollection_GetLevelByIndex(const DataCollection *collection, int index);
  return DataCollection_GetLevelByIndex(collection, index);
}

const Collection*
DataCollection_GetInstance() {
  static const Collection base = {
    .GetLevelCount = GetLevelCount,
    .GetLevelByIndex = GetLevelByIndex,
  };

  return &base;
}

int
DataCollection_GetLevelCount(const DataCollection *collection) {
  return collection->count;
}

static const unsigned char*
DataCollection_GetReadBuffer(const DataCollection *collection) {
  const unsigned char *buffer = (const unsigned char *) collection;
  const int header = sizeof(DataCollection);
  return &buffer[header];
}

static unsigned char*
DataCollection_GetWriteBuffer(DataCollection *collection) {
  unsigned char *buffer = (unsigned char *) collection;
  const int header = sizeof(DataCollection);
  return &buffer[header];
}

Binv1Level*
DataCollection_GetLevelByIndex(
    const DataCollection *collection,
    int index)
{
  static Buffer buffer = {0};
  static Binv1Level level = {0};

  if (index < collection->count && index >= 0) {
    int length = collection->allocations[index];
    int offset = 0;

    for (int i = 0; i < index; i++) {
      offset += collection->allocations[i];
    }

    const unsigned char *data = DataCollection_GetReadBuffer(collection);

    DataSource *source = Buffer_Wrap(&buffer, &data[offset], length);
    Binv1Level_From(&level, source);

    return &level;
  }

  return NULL;
}

static bool
DataCollection_CanAllocate(
    const DataCollection *collection,
    int offset,
    int length)
{
  const int header = sizeof(DataCollection);
  const int size = collection->length;
  const int limit = size - header;

  return offset + length <= limit;
}

bool
DataCollection_AddLevel(
    DataCollection *collection,
    const Binv1Level *level)
{
  unsigned int index = collection->count;
  if (index >= length(collection->allocations)) {
    return false;
  }

  int offset = 0;
  for (int i = 0; i < collection->count; i++) {
    offset += collection->allocations[i];
  }

  DataSource *source = level->source;
  Reader *reader = DataSource_AsReader(source);

  int length = Reader_GetLength(reader);
  if (!DataCollection_CanAllocate(collection, offset, length)) {
    return false;
  }

  unsigned char *data = DataCollection_GetWriteBuffer(collection);

  DataSource *target = Buffer_From(&(Buffer) {0}, &data[offset], length);
  Writer *writer = DataSource_AsWriter(target);

  for (int i = 0; i < length; i++) {
    int byte = Reader_ReadOne(reader);
    if (byte < 0) {
      return false;
    }

    if (!Writer_WriteOne(writer, byte)) {
      return false;
    }
  }

  collection->allocations[index] = length;
  collection->count++;

  return true;
}

bool
DataCollection_FindSignature(const Reader *reader) {
  const int length = sizeof(collection->signature);
  int index = 0;

  do {
    int byte = Reader_ReadOne(reader);
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
DataCollection_ReadFrom(
    DataCollection *collection,
    const Reader *reader)
{
  unsigned int index = sizeof(collection->signature);

  if (!DataCollection_FindSignature(reader)) {
    return false;
  }

  unsigned char *buffer = (unsigned char *) collection;
  const unsigned int header = sizeof(*collection);
  const unsigned int length = collection->length;

  while (index < header) {
    int byte = Reader_ReadOne(reader);
    if (byte < 0) {
      return false;
    }

    buffer[index++] = byte;
  }

  if (collection->length > length) {
    return false;
  }

  const unsigned int limit = collection->length;
  do {
    int byte = Reader_ReadOne(reader);
    if (byte < 0) {
      return false;
    }

    buffer[index++] = byte;
  } while (index != limit);

  return true;
}

bool
DataCollection_WriteTo(
    const DataCollection *collection,
    const Writer *writer)
{
  const unsigned char *buffer = (const unsigned char *) collection;
  const unsigned int length = collection->length;

  unsigned int index = sizeof(collection->signature);
  while (index < length) {
    if (!Writer_WriteOne(writer, buffer[index++])) {
      return false;
    }
  }

  return true;
}
