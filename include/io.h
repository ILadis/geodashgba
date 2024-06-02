#ifndef IO_H
#define IO_H

#include <types.h>

typedef struct Reader {
  void *self;
  bool (*Read)(void *self, void *buffer, unsigned int length);
  bool (*SeekTo)(void *self, unsigned int position);
  unsigned int (*GetPosition)(void *self);
  unsigned int (*GetLength)(void *self);
} Reader;

static inline int
Reader_ReadOne(const Reader *reader) {
  unsigned char byte = 0;
  return reader->Read(reader->self, &byte, 1) ? byte : -1;
}

static inline bool
Reader_Read(const Reader *reader, void *buffer, unsigned int length) {
  return reader->Read(reader->self, buffer, length);
}

static inline bool
Reader_ReadInt32(const Reader *reader, int *value) {
  return Reader_Read(reader, value, 4);
}

static inline bool
Reader_ReadUInt32(const Reader *reader, unsigned int *value) {
  return Reader_Read(reader, value, 4);
}

static inline bool
Reader_ReadInt16(const Reader *reader, short *value) {
  return Reader_Read(reader, value, 2);
}

static inline bool
Reader_ReadUInt16(const Reader *reader, unsigned short *value) {
  return Reader_Read(reader, value, 2);
}

static inline bool
Reader_ReadInt8(const Reader *reader, char *value) {
  return Reader_Read(reader, value, 1);
}

static inline bool
Reader_ReadUInt8(const Reader *reader, unsigned char *value) {
  return Reader_Read(reader, value, 1);
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
  bool (*Write)(void *self, void *buffer, unsigned int length);
  bool (*SeekTo)(void *self, unsigned int position);
} Writer;

static inline bool
Writer_WriteOne(const Writer *writer, unsigned char byte) {
  return writer->Write(writer->self, &byte, 1);
}

static inline bool
Writer_Write(const Writer *writer, void *buffer, unsigned int length) {
  return writer->Write(writer->self, buffer, length);
}

static inline bool
Writer_WriteInt32(const Writer *writer, int value) {
  return Writer_Write(writer, &value, 4);
}

static inline bool
Writer_WriteUInt32(const Writer *writer, unsigned int value) {
  return Writer_Write(writer, &value, 4);
}

static inline bool
Writer_WriteInt16(const Writer *writer, short value) {
  return Writer_Write(writer, &value, 2);
}

static inline bool
Writer_WriteUInt16(const Writer *writer, unsigned short value) {
  return Writer_Write(writer, &value, 2);
}

static inline bool
Writer_WriteInt8(const Writer *writer, char value) {
  return Writer_Write(writer, &value, 1);
}

static inline bool
Writer_WriteUInt8(const Writer *writer, unsigned char value) {
  return Writer_Write(writer, &value, 1);
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
  unsigned int length, position;
  union {
    const unsigned char *read;
    unsigned char *write;
  } data;
  DataSource source;
} Buffer;

DataSource*
Buffer_Wrap(
    Buffer *buffer,
    const void *data,
    unsigned int length);

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
#include <stdarg.h>

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

int
Writer_Printf(
    Writer *writer,
    const char *format, ...);

#endif
#endif
