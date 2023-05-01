
#include <disk.h>

static inline bool
Disk_BufferFill(Disk *disk, unsigned int sector) {
  return disk->read(sector, disk->buffer);
}

static inline bool
Disk_BufferCompare(
    Disk *disk,
    unsigned int offset,
    char *data)
{
  for (int i = 0; data[i] != '\0'; i++) {
    if (disk->buffer[offset + i] != data[i]) {
      return false;
    }
  }

  return true;
}

static inline unsigned int
Disk_BufferGetU32At(Disk *disk, unsigned int offset) {
  return 0
    | (disk->buffer[offset + 0] <<  0)
    | (disk->buffer[offset + 1] <<  8)
    | (disk->buffer[offset + 2] << 16)
    | (disk->buffer[offset + 3] << 24);
}

static inline unsigned short
Disk_BufferGetU16At(Disk *disk, unsigned int offset) {
  return 0
    | (disk->buffer[offset + 0] <<  0)
    | (disk->buffer[offset + 1] <<  8);
}

static inline unsigned char
Disk_BufferGetU8At(Disk *disk, unsigned int offset) {
  return 0
    | (disk->buffer[offset + 0] <<  0);
}

bool
Disk_Initialize(
    Disk *disk,
    Disk_ReadFn read)
{
  disk->offset = 0;
  disk->read = read;

  if (!Disk_BufferFill(disk, 0)) {
    return false;
  }

  // invalid master boot record
  if (Disk_BufferGetU16At(disk, 0x01FE) != 0xAA55) {
    return false;
  }

  if (Disk_BufferGetU8At(disk, 0x01C2) != 0x0C /*FAT32 with LBA*/) {
    return false;
  }

  // get first partition LBA
  unsigned int offset = Disk_BufferGetU32At(disk, 0x01C6);
  if (!Disk_BufferFill(disk, offset)) {
    return false;
  }

  // invalid super block
  if (Disk_BufferGetU16At(disk, 0x01FE) != 0xAA55) {
    return false;
  }

  // not a FAT32 file system
  if (!Disk_BufferCompare(disk, 0x0052, "FAT32")) {
    return false;
  }

  disk->bytesPerSector = Disk_BufferGetU16At(disk, 0x000B);
  disk->sectorsPerCluster = Disk_BufferGetU8At(disk, 0x000D);
  disk->numberOfReservedSectors = Disk_BufferGetU16At(disk, 0x000E);
  disk->numberOfFATs = Disk_BufferGetU8At(disk, 0x0010);
  disk->numberOfSectorsPerFAT = Disk_BufferGetU16At(disk, 0x0024);
  disk->clusterOfRootDir = Disk_BufferGetU32At(disk, 0x002C);

  unsigned int rootOffset = offset
    + disk->numberOfReservedSectors
    + disk->numberOfFATs * disk->numberOfSectorsPerFAT;

  disk->rootDirectory = rootOffset;

/*
  unsigned int sectorSize = disk->meta.bytesPerSector;
  unsigned int clusterSize = sectorSize * disk->meta.sectorsPerCluster;

  unsigned int fatAddress = 0
    + offset * 512
    + disk->meta.numberOfReservedSectors * sectorSize;

  unsigned int rootDirAddress = 0
    + offset * 512
    + disk->meta.numberOfReservedSectors * sectorSize
    + disk->meta.numberOfFATs * disk->meta.numberOfSectorsPerFAT * sectorSize;
*/

  return true;
}

bool
Disk_OpenDirectory(
    Disk *disk,
    char *path[])
{
  unsigned int offset = disk->rootDirectory;
  if (!Disk_BufferFill(disk, offset)) {
    return false;
  }

  int index = 0;
  while (path[index] != NULL) {
    char *name = path[index++];

    DiskEntry entry = {0};
    do {
      if (!Disk_ReadDirectory(disk, &entry)) {
        return false;
      }

      if (entry.type != DISK_ENTRY_DIRECTORY) {
        return false;
      }
    } while (!DiskEntry_NameEquals(&entry, name));

    unsigned int offset = disk->rootDirectory + (entry.startCluster - disk->clusterOfRootDir) * disk->sectorsPerCluster;
    if (!Disk_BufferFill(disk, offset)) {
      return false;
    }
  }

  return true;
}

bool
Disk_ReadDirectory(
    Disk *disk,
    DiskEntry *entry)
{
  unsigned char *buffer = NULL;
  unsigned int offset = disk->offset;
  unsigned char flags = 0;

  next: {
    buffer = &disk->buffer[offset];
    if (*buffer == 0x00) {
      return false;
    }

    flags = Disk_BufferGetU8At(disk, offset + 11);

    // skip hidden or system files
    if (flags & 0b00110) {
      offset += 32;
      goto next;
    }
  }

  for (int i = 0; i < length(entry->name); i++) {
    entry->name[i] = buffer[i];
  }

  entry->type = flags & 0b10000 ? DISK_ENTRY_DIRECTORY : DISK_ENTRY_FILE;
  entry->startCluster = Disk_BufferGetU16At(disk, offset + 26);
  entry->fileSize = Disk_BufferGetU32At(disk, offset + 28);

  disk->offset = offset + 32;

  return true;
}

bool
DiskEntry_NameEquals(
    DiskEntry *entry,
    char *name)
{
  for (int i = 0; i < length(entry->name); i++) {
    if (name[i] == '\0' || name[i] != entry->name[i]) {
      return false;
    }
  }

  return true;
}
