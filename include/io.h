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

typedef struct DataReader {
  unsigned char *data;
  int position, length;
  Reader base;
} DataReader;

bool
DataReader_From(
    DataReader *stream,
    void *data, int length);

int
DataReader_Read(void *reader);

static inline Reader*
DataReader_AsReader(DataReader *reader) {
  return &reader->base;
}

#ifdef NOGBA
#include <stdio.h>

typedef struct FileReader {
  FILE *fp;
  Reader base;
} FileReader;

bool
FileReader_Open(
    FileReader *stream,
    const char *path,
    const char *modes);

bool
FileReader_From(
    FileReader *stream,
    FILE *fp);

int
FileReader_Read(void *reader);

static inline Reader*
FileReader_AsReader(FileReader *reader) {
  return &reader->base;
}

#endif
#endif
