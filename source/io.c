
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
    int byte = Reader_ReadOne(reader);
    if (byte < 0) {
      break;
    }

    if (!Writer_WriteOne(writer, byte)) {
      break;
    }

    length++;
  } while (true);

  Reader_SeekTo(reader, 0);
  Writer_SeekTo(writer, 0);

  return length;
}

DataSource*
Buffer_FromString(
    Buffer *buffer,
    char *string)
{
  unsigned int length = 0;
  while (string[length] != '\0') {
    length++;
  }

  return Buffer_From(buffer, string, length);
}

static bool
Buffer_Read(
    void *self,
    void *data,
    unsigned int length)
{
  Buffer *buffer = self;
  unsigned int offset = buffer->position;

  if (offset + length > buffer->length) {
    return false;
  }

  unsigned char *values = data;
  for (unsigned int index = 0; index < length; index++) {
    values[index] = buffer->data.read[offset + index];
  }

  buffer->position += length;

  return true;
}

static bool
Buffer_NoopWrite(
    unused void *buffer,
    unused const void *data,
    unused unsigned int length)
{
  return false;
}

bool
Buffer_Write(
    void *self,
    const void *data,
    unsigned int length)
{
  Buffer *buffer = self;
  unsigned int offset = buffer->position;

  if (offset + length > buffer->length) {
    return false;
  }

  const unsigned char *values = data;
  for (unsigned int index = 0; index < length; index++) {
    buffer->data.write[offset + index] = values[index];
  }

  buffer->position += length;

  return true;
}

static bool
Buffer_SeekTo(
    void *self,
    unsigned int position)
{
  Buffer *buffer = self;
  // allow seeking to end of buffer (where next read would return -1)
  if (position > buffer->length) {
    return false;
  }

  buffer->position = position;
  return true;
}

static unsigned int
Buffer_GetLength(void *self) {
  Buffer *buffer = self;
  return buffer->length;
}

static unsigned int
Buffer_GetPosition(void *self) {
  Buffer *buffer = self;
  return buffer->position;
}

DataSource*
Buffer_Wrap(
    Buffer *buffer,
    const void *data,
    unsigned int length)
{
  if (data == NULL) {
    return NULL;
  }

  Reader *reader = &buffer->source.reader;
  reader->self = buffer;
  reader->Read = Buffer_Read;
  reader->GetLength = Buffer_GetLength;
  reader->GetPosition = Buffer_GetPosition;
  reader->SeekTo = Buffer_SeekTo;

  Writer *writer = &buffer->source.writer;
  writer->self = buffer;
  writer->Write = Buffer_NoopWrite;
  writer->SeekTo = Buffer_SeekTo;

  buffer->data.read = data;
  buffer->position = 0;
  buffer->length = length;

  return &buffer->source;
}

DataSource*
Buffer_From(
    Buffer *buffer,
    void *data,
    unsigned int length)
{
  DataSource *source = Buffer_Wrap(buffer, data, length);

  if (source == NULL) {
    return NULL;
  }

  // make buffer writeable
  Writer *writer = &source->writer;
  writer->Write = Buffer_Write;
  buffer->data.write = data;

  return source;
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

static bool
File_Read(
    void *self,
    void *data,
    unsigned int length)
{
  File *file = self;
  FILE *fp = file->fp;
  return fread(data, 1, length, fp) == length;
}

static bool
File_Write(
    void *self,
    const void *data,
    unsigned int length)
{
  File *file = self;
  FILE *fp = file->fp;
  return fwrite(data, 1, length, fp) == length;
}

static bool
File_SeekTo(
    void *self,
    unsigned int position)
{
  File *file = self;
  FILE *fp = file->fp;
  return fseek(fp, position, SEEK_SET) == 0;
}

static unsigned int
File_GetLength(void *self) {
  File *file = self;
  FILE *fp = file->fp;

  long position = ftell(fp);
  if (position < 0) {
    return 0;
  }

  fseek(fp, 0, SEEK_END);
  long length = ftell(fp);
  if (length < 0) {
    return 0;
  }

  fseek(fp, position, SEEK_SET);
  return (unsigned int) length;
}

static unsigned int
File_GetPosition(void *self) {
  File *file = self;
  FILE *fp = file->fp;

  long position = ftell(fp);
  if (position < 0) {
    return 0;
  }

  return (unsigned int) position;
}

DataSource*
File_From(
    File *file,
    FILE *fp)
{
  if (fp == NULL) {
    return NULL;
  }

  Reader *reader = &file->source.reader;
  reader->self = file;
  reader->Read = File_Read;
  reader->SeekTo = File_SeekTo;
  reader->GetLength = File_GetLength;
  reader->GetPosition = File_GetPosition;

  Writer *writer = &file->source.writer;
  writer->self = file;
  writer->Write = File_Write;
  writer->SeekTo = File_SeekTo;

  file->fp = fp;

  return &file->source;
}

int
Writer_Printf(
    Writer *writer,
    const char *format, ...)
{
  int result = -1;

  va_list arguments;
  va_start(arguments, format);

  if (writer->Write == File_Write) {
    File *file = writer->self;
    result = vfprintf(file->fp, format, arguments);
  }

  if (writer->Write == Buffer_Write) {
    Buffer *buffer = writer->self;

    void *data = &buffer->data.write[buffer->position];
    unsigned int length = buffer->length - buffer->position;

    result = vsnprintf(data, length, format, arguments);
    buffer->position += result < 0 ? 0 : result;
  }

  va_end(arguments);

  return result;
}

#endif

static bool
Base16_Read(
    void *self,
    void *data,
    unsigned int length)
{
  Base16 *base = self;
  Reader *reader = base->delegate.reader;

  unsigned char *values = data;
  for (unsigned int index = 0; index < length; index++) {
    int upper = Reader_ReadOne(reader);
    int lower = Reader_ReadOne(reader);

    if (lower < 0 || upper < 0) {
      return false;
    }

    unsigned char value = (hexto(upper) << 4) | hexto(lower);
    values[index] = value;
  }

  return true;
}

bool
Base16_Write(
    void *self,
    const void *data,
    unsigned int length)
{
  Base16 *base = self;
  Writer *writer = base->delegate.writer;

  const unsigned char *values = data;
  for (unsigned int index = 0; index < length; index++) {
    unsigned char value = values[index];

    unsigned char upper = hexof(value, 1);
    unsigned char lower = hexof(value, 0);

    if (!Writer_WriteOne(writer, upper) || !Writer_WriteOne(writer, lower)) {
      return false;
    }
  }

  return true;
}

static bool
Base16_SeekTo(
    void *self,
    unsigned int position)
{
  Base16 *base = self;

  Reader *reader = base->delegate.reader;
  return Reader_SeekTo(reader, position << 1);
}

static unsigned int
Base16_GetLength(void *self) {
  Base16 *base = self;

  Reader *reader = base->delegate.reader;
  unsigned int length = Reader_GetLength(reader);

  return length >> 1;
}

static unsigned int
Base16_GetPosition(void *self) {
  Base16 *base = self;

  Reader *reader = base->delegate.reader;
  unsigned int position = Reader_GetPosition(reader);

  return position >> 1;
}

DataSource*
Base16_Of(
    Base16 *base,
    DataSource *source)
{
  if (source == NULL) {
    return NULL;
  }

  base->delegate.reader = DataSource_AsReader(source);
  base->delegate.writer = DataSource_AsWriter(source);

  Reader *reader = &base->source.reader;
  reader->self = base;
  reader->Read = Base16_Read;
  reader->GetLength = Base16_GetLength;
  reader->GetPosition = Base16_GetPosition;
  reader->SeekTo = Base16_SeekTo;

  Writer *writer = &base->source.writer;
  writer->self = base;
  writer->Write = Base16_Write;
  writer->SeekTo = Base16_SeekTo;

  return &base->source;
}
