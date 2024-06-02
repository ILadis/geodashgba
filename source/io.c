
#include <io.h>

static inline bool
IO_IsLittleEndian() {
  const short int value = 1;
  const char *endian = (char *) &value;

  return *endian == 1;
}

bool
Reader_ReadValue(
    const Reader *reader,
    void *value,
    unsigned int length)
{
#ifdef NOGBA
  if (!IO_IsLittleEndian()) {
    // big endian is unsupported
    return false;
  }
#endif

  unsigned char *values = value;

  for (unsigned int i = 0; i < length; i++) {
    int byte = Reader_Read(reader);
    if (byte < 0) {
      return false;
    }

    values[i] = byte;
  }

  return true;
}

bool
Writer_WriteValue(
    const Writer *writer,
    void *value,
    unsigned int length)
{
#ifdef NOGBA
  if (!IO_IsLittleEndian()) {
    // big endian is unsupported
    return false;
  }
#endif

  unsigned char *values = value;

  for (unsigned int i = 0; i < length; i++) {
    if (!Writer_Write(writer, values[i])) {
      return false;
    }
  }

  return true;
}

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

static int
Buffer_Read(void *self) {
  Buffer *buffer = self;
  unsigned int index = buffer->position;

  if (index >= buffer->length) {
    return -1;
  }

  int byte = buffer->data.read[index];
  buffer->position++;

  return byte;
}

static bool
Buffer_NoopWrite(
    unused void *buffer,
    unused unsigned char byte)
{
  return false;
}

bool
Buffer_Write(
    void *self,
    unsigned char byte)
{
  Buffer *buffer = self;
  unsigned int index = buffer->position;

  if (index >= buffer->length) {
    return false;
  }

  buffer->data.write[index] = byte;
  buffer->position++;

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

  bool Buffer_Write(void *buffer, unsigned char byte);

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

static int
File_Read(void *self) {
  File *file = self;
  FILE *fp = file->fp;
  return fgetc(fp);
}

static bool
File_Write(
    void *self,
    unsigned char byte)
{
  File *file = self;
  FILE *fp = file->fp;
  return fputc(byte, fp) == byte;
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

  bool File_Write(void *file, unsigned char byte);
  if (writer->Write == File_Write) {
    File *file = writer->self;
    result = vfprintf(file->fp, format, arguments);
  }

  bool Buffer_Write(void *buffer, unsigned char byte);
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
