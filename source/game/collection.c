
#include <game/collection.h>

const Collection *collection = Collection_DefineNew(300 * 1024);

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

Level*
Collection_GetLevelByIndex(
    const Collection *collection,
    int index)
{
  static Level level = { .format = FORMAT_BINV1 };

  if (index < collection->count && index >= 0) {
    int length = collection->allocations[index];
    int offset = 0;

    for (int i = 0; i < index; i++) {
      offset += collection->allocations[i];
    }

    const unsigned char *buffer = Collection_GetReadBuffer(collection);

    level.size.x = length;
    level.buffer.read = &buffer[offset];

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
    const Level *level)
{
  // only binary levels can be added
  if (level->format != FORMAT_BINV1) {
    return false;
  }

  int index = collection->count;
  if (index >= length(collection->allocations)) {
    return false;
  }

  int offset = 0;
  for (int i = 0; i < collection->count; i++) {
    offset += collection->allocations[i];
  }

  int length = level->size.x;
  if (!Collection_CanAllocate(collection, offset, length)) {
    return false;
  }

  unsigned char *buffer = Collection_GetWriteBuffer(collection);
  const unsigned char *data = level->buffer.read;

  for (int i = 0; i < length; i++) {
    buffer[offset + i] = data[i];
  }

  collection->allocations[index] = length;
  collection->count++;

  return true;
}
