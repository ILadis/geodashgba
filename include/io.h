#ifndef STREAM_H
#define STREAM_H

#include <types.h>

typedef struct Reader {
  void *self;
  int  (*Read)(void *self);
  bool (*SeekTo)(void *self, int position);
} Reader;

static inline int
Reader_Read(const Reader *reader) {
  return reader->Read(reader->self);
}

static inline bool
Reader_SeekTo(const Reader *reader, int position) {
  return reader->SeekTo(reader->self, position);
}

typedef struct Writer {
  void *self;
  bool (*Write)(void *self, unsigned char byte);
  bool (*SeekTo)(void *self, int position);
} Writer;

static inline bool
Writer_Write(const Writer *writer, unsigned char byte) {
  return writer->Write(writer->self, byte);
}

static inline bool
Writer_SeekTo(const Writer *writer, int position) {
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
  int length, position;
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
    int length);

DataSource*
Buffer_AsDataSource(Buffer *buffer);

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
