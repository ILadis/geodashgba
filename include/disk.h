#ifndef DISK_H
#define DISK_H

#include <types.h>
#include <math.h>
#include <io.h>

typedef bool (*Disk_ReadFn)(unsigned int sector, void *buffer);

typedef struct DiskEntry {
  char name[12];
  enum {
    DISK_ENTRY_FILE,
    DISK_ENTRY_DIRECTORY,
  } type;
  unsigned int startCluster;
  unsigned int fileSize;
} DiskEntry;

typedef struct Disk {
  unsigned char buffer[512]; // TODO support sectors greater than 512 bytes
  bool (*read)(unsigned int sector, void *buffer);

  struct DiskInfo {
    unsigned short sectorSize;
    unsigned short clusterSize;

    unsigned int rootDirCluster;
    unsigned int firstDataSector;
    unsigned int firstFATSector;
  } info;

  struct DataSource source;
  struct DiskEntry entry;
  struct DiskReader {
    unsigned int cluster;
    unsigned int sector;
    unsigned int position;
    unsigned int size;
  } reader;
} Disk;

#define Disk_EntrySize   0x20
#define Disk_ClusterMask 0x0FFFFFFF
#define Disk_BadCluster  0x0FFFFFF7

typedef struct DiskInfo DiskInfo;
typedef struct DiskReader DiskReader;

static inline unsigned int
DiskInfo_BytesPerSector(DiskInfo *info) {
  return 1 << info->sectorSize;
}

static inline unsigned int
DiskInfo_BytesPerCluster(DiskInfo *info) {
  return 1 << (info->sectorSize + info->clusterSize);
}

static inline unsigned int
DiskInfo_SectorsPerCluster(DiskInfo *info) {
  return 1 << info->clusterSize;
}

bool
Disk_Initialize(
    Disk *disk,
    Disk_ReadFn read);

DataSource*
Disk_OpenFile(
    Disk *disk,
    const DiskEntry *entry);

bool
Disk_OpenDirectory(
    Disk *disk,
    char *path[]);

bool
Disk_ReadDirectory(
    Disk *disk,
    DiskEntry *entry);

bool
DiskEntry_NameEquals(
    const DiskEntry *entry,
    char *name);

bool
DiskEntry_NormalizePath(
    const char *pathname,
    char *path[]);

char*
DiskEntry_DirnameOf(char *path[]);

#endif
