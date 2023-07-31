
#include <io.h>

int
DataSource_CopyFrom(
    DataSource *dst,
    DataSource *src)
{
  int length = 0;

  Reader *reader = DataSource_AsReader(src);
  Writer *writer = DataSource_AsWriter(dst);

  do {
    int byte = Reader_Read(reader);
    if (byte < 0) {
      break;
    }

    if (!Writer_Write(writer, byte)) {
      break;
    }

    length++;
  } while (true);

  Reader_SeekTo(reader, 0);
  Writer_SeekTo(writer, 0);

  return length;
}

DataSource*
Buffer_From(
    Buffer *buffer,
    void *data,
    int length)
{
  buffer->data = data;

  if (data == NULL) {
    return NULL;
  }

  buffer->position = 0;
  buffer->length = length;

  return Buffer_AsDataSource(buffer);
}

DataSource*
Buffer_AsDataSource(Buffer *buffer) {
  int Buffer_Read(void *buffer);
  bool Buffer_Write(void *buffer, unsigned char byte);
  bool Buffer_SeekTo(void *buffer, int position);

  Reader *reader = &buffer->source.reader;
  reader->self = buffer;
  reader->Read = Buffer_Read;
  reader->SeekTo = Buffer_SeekTo;

  Writer *writer = &buffer->source.writer;
  writer->self = buffer;
  writer->Write = Buffer_Write;
  writer->SeekTo = Buffer_SeekTo;

  return &buffer->source;
}

int
Buffer_Read(void *self) {
  Buffer *buffer = self;
  int index = buffer->position;

  if (index >= buffer->length) {
    return -1;
  }

  int byte = buffer->data[index];
  buffer->position++;

  return byte;
}

bool
Buffer_Write(
    void *self,
    unsigned char byte)
{
  Buffer *buffer = self;
  int index = buffer->position;

  if (index >= buffer->length) {
    return false;
  }

  buffer->data[index] = byte;
  buffer->position++;

  return true;
}

bool
Buffer_SeekTo(
    void *self,
    int position)
{
  Buffer *buffer = self;
  if (position >= buffer->length) {
    return false;
  }

  buffer->position = position;
  return true;
}

#ifdef NOGBA

DataSource*
File_Open(
    File *file,
    const char *path,
    const char *modes)
{
  FILE *fp = fopen(path, modes);
  if (fp == NULL) {
    return false;
  }

  return File_From(file, fp);
}

DataSource*
File_From(
    File *file,
    FILE *fp)
{
  file->fp = fp;

  if (fp == NULL) {
    return NULL;
  }

  int File_Read(void *file);
  bool File_Write(void *file, unsigned char byte);
  bool File_SeekTo(void *file, int position);

  Reader *reader = &file->source.reader;
  reader->self = file;
  reader->Read = File_Read;
  reader->SeekTo = File_SeekTo;

  Writer *writer = &file->source.writer;
  writer->self = file;
  writer->Write = File_Write;
  writer->SeekTo = File_SeekTo;

  return &file->source;
}

int
File_Read(void *self) {
  File *file = self;
  FILE *fp = file->fp;
  return fgetc(fp);
}

bool
File_Write(
    void *self,
    unsigned char byte)
{
  File *file = self;
  FILE *fp = file->fp;
  return fputc(byte, fp) == byte;
}

bool
File_SeekTo(
    void *self,
    int position)
{
  File *file = self;
  FILE *fp = file->fp;
  return fseek(fp, position, SEEK_SET) == 0;
}

#endif
