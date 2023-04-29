
#include <disk.h>

bool
Disk_Initialize(Disk *disk) {
  if (!Disk_BufferFill(disk, 0)) {
    return false;
  }

  // invalid master boot record
  if (Disk_BufferGetU16(disk, 0x01FE) != 0xAA55) {
    return false;
  }

  if (Disk_BufferGetU8(disk, 0x01C2) != 0x0C /*FAT32 with LBA*/) {
    return false;
  }

  // get first partition LBA
  unsigned int offset = Disk_BufferGetU32(disk, 0x01C6);
  if (!Disk_BufferFill(disk, offset)) {
    return false;
  }

  // invalid super block
  if (Disk_BufferGetU16(disk, 0x01FE) != 0xAA55) {
    return false;
  }

  // not a FAT32 file system
  if (!Disk_BufferCompare(disk, 0x0052, "FAT32")) {
    return false;
  }

  disk->meta.bytesPerSector = Disk_BufferGetU16(disk, 0x000B);
  disk->meta.sectorsPerCluster = Disk_BufferGetU8(disk, 0x000D);
  disk->meta.numberOfReservedSectors = Disk_BufferGetU16(disk, 0x000E);
  disk->meta.numberOfFATs = Disk_BufferGetU8(disk, 0x0010);
  disk->meta.numberOfSectorsPerFAT = Disk_BufferGetU16(disk, 0x0024);
  disk->meta.clusterOfRootDir = Disk_BufferGetU32(disk, 0x002C);

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

  printf("FAT addr at: %x\n", fatAddress);
  printf("rootDir addr at: %x\n", rootDirAddress);
*/

  return true;
}
