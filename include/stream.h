#ifndef STREAM_H
#define STREAM_H

#include <types.h>

typedef struct Reader {
  void *self;
  int (*read)(void *self);
} Reader;

static inline int
Reader_ReadNext(const Reader *reader) {
  return reader->read(reader->self);
}

typedef struct DataStream {
  unsigned char *data;
  int position, length;
  Reader reader;
} DataStream;

bool
DataStream_From(
    DataStream *stream,
    void *data, int length);

int
DataStream_Read(void *reader);

static inline Reader*
DataStream_AsReader(DataStream *stream) {
  return &stream->reader;
}

#ifdef NOGBA
#include <stdio.h>

typedef struct FileStream {
  FILE *fp;
  Reader reader;
} FileStream;

bool
FileStream_Open(
    FileStream *stream,
    const char *path,
    const char *modes);

bool
FileStream_From(
    FileStream *stream,
    FILE *fp);

int
FileStream_Read(void *reader);

static inline Reader*
FileStream_AsReader(FileStream *stream) {
  return &stream->reader;
}

#endif
#endif
