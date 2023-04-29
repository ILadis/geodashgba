#ifndef DISK_H
#define DISK_H

#include "types.h"

typedef bool (*Disk_ReadFn)(unsigned int sector, void *buffer);

typedef struct Disk {
  unsigned char buffer[512];
  bool (*read)(unsigned int sector, void *buffer);
  struct {
    unsigned short bytesPerSector;
    unsigned short sectorsPerCluster;
    unsigned short numberOfReservedSectors;
    unsigned short numberOfFATs;
    unsigned short numberOfSectorsPerFAT;
    unsigned short clusterOfRootDir;
  } meta;
} Disk;

static inline bool
Disk_BufferFill(Disk *disk, unsigned int sector) {
  return disk->read(sector, disk->buffer);
}

static inline bool
Disk_BufferCompare(Disk *disk, unsigned int offset, char *data) {
  for (int i = 0; data[i] != '\0'; i++) {
    if (disk->buffer[offset + i] != data[i]) {
      return false;
    }
  }

  return true;
}

static inline unsigned int
Disk_BufferGetU32(Disk *disk, unsigned int offset) {
  return 0
    | (disk->buffer[offset + 0] <<  0)
    | (disk->buffer[offset + 1] <<  8)
    | (disk->buffer[offset + 2] << 16)
    | (disk->buffer[offset + 3] << 24);
}

static inline unsigned short
Disk_BufferGetU16(Disk *disk, unsigned int offset) {
  return 0
    | (disk->buffer[offset + 0] <<  0)
    | (disk->buffer[offset + 1] <<  8);
}

static inline unsigned char
Disk_BufferGetU8(Disk *disk, unsigned int offset) {
  return 0
    | (disk->buffer[offset + 0] <<  0);
}

static inline void
Disk_ReadFrom(Disk *disk, Disk_ReadFn read) {
  disk->read = read;
}

bool
Disk_Initialize(Disk *disk);

#endif
