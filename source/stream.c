
#include <stream.h>

bool
DataStream_From(
    DataStream *stream,
    void *data, int length)
{
  stream->data = data;

  if (data == NULL) {
    return false;
  }

  stream->position = 0;
  stream->length = length;
  stream->reader.self = stream;
  stream->reader.read = DataStream_Read;

  return true;
}

int
DataStream_Read(void *reader) {
  DataStream *stream = reader;
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
FileStream_Open(
    FileStream *stream,
    const char *path,
    const char *modes)
{
  FILE *fp = fopen(path, modes);
  if (fp == NULL) {
    return false;
  }

  stream->fp = fp;
  stream->reader.self = stream;
  stream->reader.read = FileStream_Read;

  return true;
}

bool
FileStream_From(
    FileStream *stream,
    FILE *fp)
{
  stream->fp = fp;

  if (fp == NULL) {
    return false;
  }

  stream->reader.self = stream;
  stream->reader.read = FileStream_Read;

  return true;
}

int
FileStream_Read(void *reader) {
  FileStream *stream = reader;
  FILE *fp = stream->fp;
  return fgetc(fp);
}

#endif
