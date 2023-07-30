
#include <io.h>

bool
DataReader_From(
    DataReader *reader,
    void *data, int length)
{
  reader->data = data;

  if (data == NULL) {
    return false;
  }

  reader->position = 0;
  reader->length = length;
  reader->base.self = reader;
  reader->base.read = DataReader_Read;

  return true;
}

int
DataReader_Read(void *reader) {
  DataReader *stream = reader;
  int index = stream->position;

  if (index >= stream->length) {
    return -1;
  }

  int byte = stream->data[index];
  stream->position++;

  return byte;
}

#ifdef NOGBA

bool
FileReader_Open(
    FileReader *reader,
    const char *path,
    const char *modes)
{
  FILE *fp = fopen(path, modes);
  if (fp == NULL) {
    return false;
  }

  reader->fp = fp;
  reader->base.self = reader;
  reader->base.read = FileReader_Read;

  return true;
}

bool
FileReader_From(
    FileReader *stream,
    FILE *fp)
{
  stream->fp = fp;

  if (fp == NULL) {
    return false;
  }

  stream->base.self = stream;
  stream->base.read = FileReader_Read;

  return true;
}

int
FileReader_Read(void *reader) {
  FileReader *stream = reader;
  FILE *fp = stream->fp;
  return fgetc(fp);
}

#endif
