#ifndef IO_H
#define IO_H

#include <types.h>

typedef struct Reader {
  void *self;
  int  (*Read)(void *self);
  bool (*SeekTo)(void *self, unsigned int position);
  unsigned int (*GetPosition)(void *self);
  unsigned int (*GetLength)(void *self);
} Reader;

static inline int
Reader_Read(const Reader *reader) {
  return reader->Read(reader->self);
}

static inline bool
Reader_ReadValue(const Reader *reader, void *value, unsigned int length) {
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

static inline bool
Reader_ReadInt32(const Reader *reader, int *value) {
  return Reader_ReadValue(reader, value, 4);
}

static inline bool
Reader_ReadUInt32(const Reader *reader, unsigned int *value) {
  return Reader_ReadValue(reader, value, 4);
}

static inline bool
Reader_ReadInt16(const Reader *reader, short *value) {
  return Reader_ReadValue(reader, value, 2);
}

static inline bool
Reader_ReadUInt16(const Reader *reader, unsigned short *value) {
  return Reader_ReadValue(reader, value, 2);
}

static inline bool
Reader_ReadInt8(const Reader *reader, char *value) {
  return Reader_ReadValue(reader, value, 1);
}

static inline bool
Reader_ReadUInt8(const Reader *reader, unsigned char *value) {
  return Reader_ReadValue(reader, value, 1);
}

static inline bool
Reader_SeekTo(const Reader *reader, unsigned int position) {
  return reader->SeekTo(reader->self, position);
}

static inline unsigned int
Reader_GetPosition(const Reader *reader) {
  return reader->GetPosition(reader->self);
}

static inline unsigned int
Reader_GetLength(const Reader *reader) {
  return reader->GetLength(reader->self);
}

typedef struct Writer {
  void *self;
  bool (*Write)(void *self, unsigned char byte);
  bool (*SeekTo)(void *self, unsigned int position);
} Writer;

static inline bool
Writer_Write(const Writer *writer, unsigned char byte) {
  return writer->Write(writer->self, byte);
}

static inline bool
Writer_SeekTo(const Writer *writer, unsigned int position) {
  return writer->SeekTo(writer->self, position);
}

typedef struct DataSource {
  Reader reader;
  Writer writer;
} DataSource;

static inline Reader*
DataSource_AsReader(DataSource *source) {
  return &source->reader;
}

static inline Writer*
DataSource_AsWriter(DataSource *source) {
  return &source->writer;
}

int
DataSource_CopyFrom(
    DataSource *dst,
    DataSource *src);

typedef struct Buffer {
  unsigned char *data;
  unsigned int length, position;
  DataSource source;
} Buffer;

#define Buffer_CreateNew(bytes...) ((Buffer) { \
  .data = (unsigned char[]) { bytes }, \
  .length = sizeof((unsigned char[]) { bytes }) \
})

DataSource*
Buffer_From(
    Buffer *buffer,
    void *data,
    unsigned int length);

DataSource*
Buffer_FromString(
    Buffer *buffer,
    char *string);

#ifdef NOGBA
#include <stdio.h>

typedef struct File {
  FILE *fp;
  DataSource source;
} File;

DataSource*
File_Open(
    File *file,
    const char *path,
    const char *modes);

DataSource*
File_From(
    File *file,
    FILE *fp);

#endif
#endif
