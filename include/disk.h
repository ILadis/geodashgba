#ifndef DISK_H
#define DISK_H

#include "types.h"

typedef bool (*Disk_ReadFn)(unsigned int sector, void *buffer, int count);

typedef struct Disk {
  unsigned char buffer[512];
  unsigned int offset;
  bool (*read)(unsigned int sector, void *buffer, int count);
  unsigned short bytesPerSector;
  unsigned short sectorsPerCluster;
  unsigned short numberOfReservedSectors;
  unsigned short numberOfFATs;
  unsigned short numberOfSectorsPerFAT;
  unsigned short clusterOfRootDir;
  unsigned int rootDirectory;
} Disk;

typedef struct DiskEntry {
  char name[12];
  enum {
    DISK_ENTRY_FILE,
    DISK_ENTRY_DIRECTORY,
  } type;
  unsigned int startCluster;
  unsigned int fileSize;
} DiskEntry;

bool
Disk_Initialize(
    Disk *disk,
    Disk_ReadFn read);

bool
Disk_OpenDirectory(
    Disk *disk,
    char *path[]);

bool
Disk_ReadDirectory(
    Disk *disk,
    DiskEntry *entry);

bool
Disk_ReadFile(
    Disk *disk,
    DiskEntry *entry,
    unsigned char *buffer);

bool
DiskEntry_NameEquals(
    DiskEntry *entry,
    char *name);

#endif
