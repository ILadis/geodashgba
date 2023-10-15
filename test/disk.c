
#include <disk.h>
#include "test.h"

static unsigned char bootRecord[] = {
  [447] = 0x82, 0x03, 0x00, 0x0c, 0xfe, 0xff, 0xff, 0x00, 0x20, 0x00, 0x00, 0x00, 0x04, 0xb7, 0x03, 0x00,
  [510] = 0x55, 0xaa
};

static unsigned char superBlock[] = {
  [  0] = 0xeb, 0x00, 0x90, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x02, 0x40, 0x92, 0x04,
          0x02, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x3f, 0x00, 0xff, 0x00, 0x00, 0x20, 0x00, 0x00,
          0x00, 0x04, 0xb7, 0x03, 0xb7, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
          0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x80, 0x00, 0x29, 0x63, 0x34, 0x34, 0x33, 0x4e, 0x4f, 0x20, 0x4e, 0x41, 0x4d, 0x45, 0x20, 0x20,
          0x20, 0x20, 0x46, 0x41, 0x54, 0x33, 0x32, 0x20, 0x20, 0x20,
  [510] = 0x55, 0xaa
};

static unsigned char rootDir[] = {
  [  0] = 0x47, 0x42, 0x41, 0x53, 0x59, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0x88, 0x87, 0xaa,
          0x3e, 0x54, 0x3e, 0x54, 0x00, 0x00, 0x87, 0xaa, 0x3e, 0x54, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x52, 0x4f, 0x4d, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0x5a, 0xe3, 0xbc,
          0x48, 0x56, 0x82, 0x56, 0x00, 0x00, 0x6a, 0x96, 0x93, 0x56, 0x2d, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x42, 0x64, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xc4, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x01, 0x62, 0x00, 0x6f, 0x00, 0x6f, 0x00, 0x74, 0x00, 0x2d, 0x00, 0x0f, 0x00, 0xc4, 0x76, 0x00,
          0x31, 0x00, 0x2e, 0x00, 0x30, 0x00, 0x35, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x75, 0x00, 0x70, 0x00,
          0x42, 0x4f, 0x4f, 0x54, 0x2d, 0x56, 0x7e, 0x31, 0x55, 0x50, 0x44, 0x20, 0x00, 0x55, 0x33, 0x9b,
          0x85, 0x53, 0x3b, 0x56, 0x00, 0x00, 0x2d, 0x9b, 0x85, 0x53, 0x1e, 0x15, 0x00, 0x70, 0x00, 0x00,
  [511] = 0x00
};

static unsigned char gbasysDir[] = {
  [  0] = 0x2e, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0xb5, 0x4a, 0x9a,
          0x85, 0x53, 0x85, 0x53, 0x00, 0x00, 0x4a, 0x9a, 0x85, 0x53, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x2e, 0x2e, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0xb5, 0x4a, 0x9a,
          0x85, 0x53, 0x85, 0x53, 0x00, 0x00, 0x4a, 0x9a, 0x85, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x41, 0x65, 0x00, 0x6d, 0x00, 0x75, 0x00, 0x00, 0x00, 0xff, 0xff, 0x0f, 0x00, 0x3c, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x45, 0x4d, 0x55, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0xb5, 0x4a, 0x9a,
          0x85, 0x53, 0x85, 0x53, 0x00, 0x00, 0x00, 0x9a, 0x85, 0x53, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x41, 0x65, 0x00, 0x6d, 0x00, 0x75, 0x00, 0x73, 0x00, 0x61, 0x00, 0x0f, 0x00, 0xf2, 0x76, 0x00,
          0x65, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x45, 0x4d, 0x55, 0x53, 0x41, 0x56, 0x45, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0xaf, 0x44, 0x88,
          0x64, 0x54, 0x64, 0x54, 0x00, 0x00, 0x94, 0x88, 0x64, 0x54, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x47, 0x42, 0x41, 0x4f, 0x53, 0x20, 0x20, 0x20, 0x47, 0x42, 0x41, 0x20, 0x00, 0x88, 0x87, 0xaa,
          0x3e, 0x54, 0x2b, 0x54, 0x00, 0x00, 0xd8, 0x64, 0x2b, 0x54, 0x30, 0x1e, 0x00, 0x00, 0x02, 0x00,
          0x41, 0x73, 0x00, 0x61, 0x00, 0x76, 0x00, 0x65, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x13, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x53, 0x41, 0x56, 0x45, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0x55, 0x6b, 0xa5,
          0x3e, 0x54, 0x3e, 0x54, 0x00, 0x00, 0x6b, 0xa5, 0x3e, 0x54, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x41, 0x73, 0x00, 0x79, 0x00, 0x73, 0x00, 0x00, 0x00, 0xff, 0xff, 0x0f, 0x00, 0x54, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x53, 0x59, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0x11, 0x90, 0xaa,
          0x3e, 0x54, 0x3e, 0x54, 0x00, 0x00, 0x90, 0xaa, 0x3e, 0x54, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x00,
  [511] = 0x00
};

static unsigned char sysDir[] = {
  [  0] = 0x2e, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0x0d, 0x4b, 0x9a,
          0x85, 0x53, 0x85, 0x53, 0x00, 0x00, 0x4b, 0x9a, 0x85, 0x53, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x2e, 0x2e, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0x0d, 0x4b, 0x9a,
          0x85, 0x53, 0x85, 0x53, 0x00, 0x00, 0x4b, 0x9a, 0x85, 0x53, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x41, 0x62, 0x00, 0x72, 0x00, 0x61, 0x00, 0x6d, 0x00, 0x2d, 0x00, 0x0f, 0x00, 0x8a, 0x64, 0x00,
          0x62, 0x00, 0x2e, 0x00, 0x64, 0x00, 0x61, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
          0x42, 0x52, 0x41, 0x4d, 0x2d, 0x44, 0x42, 0x20, 0x44, 0x41, 0x54, 0x20, 0x00, 0x11, 0x90, 0xaa,
          0x3e, 0x54, 0x6c, 0x51, 0x00, 0x00, 0x5c, 0x47, 0x6c, 0x51, 0x34, 0x1e, 0xd7, 0x37, 0x00, 0x00,
          0x41, 0x72, 0x00, 0x6f, 0x00, 0x6d, 0x00, 0x63, 0x00, 0x66, 0x00, 0x0f, 0x00, 0x97, 0x67, 0x00,
          0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
          0x52, 0x4f, 0x4d, 0x43, 0x46, 0x47, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0x0d, 0x4b, 0x9a,
          0x85, 0x53, 0x3e, 0x54, 0x00, 0x00, 0xf7, 0xa6, 0x3e, 0x54, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x41, 0x72, 0x00, 0x65, 0x00, 0x67, 0x00, 0x69, 0x00, 0x73, 0x00, 0x0f, 0x00, 0xbc, 0x74, 0x00,
          0x65, 0x00, 0x72, 0x00, 0x79, 0x00, 0x2e, 0x00, 0x64, 0x00, 0x00, 0x00, 0x61, 0x00, 0x74, 0x00,
          0x52, 0x45, 0x47, 0x49, 0x53, 0x54, 0x7e, 0x31, 0x44, 0x41, 0x54, 0x20, 0x00, 0x00, 0x00, 0x00,
          0x17, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x4f, 0x1f, 0x15, 0x70, 0x01, 0x00, 0x00,
          0x52, 0x45, 0x43, 0x45, 0x4e, 0x54, 0x20, 0x20, 0x44, 0x41, 0x54, 0x20, 0x18, 0x00, 0x00, 0x00,
          0x17, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x4f, 0x20, 0x15, 0xde, 0x12, 0x00, 0x00,
          0x41, 0x72, 0x00, 0x61, 0x00, 0x6d, 0x00, 0x64, 0x00, 0x75, 0x00, 0x0f, 0x00, 0x1f, 0x6d, 0x00,
          0x70, 0x00, 0x30, 0x00, 0x31, 0x00, 0x2e, 0x00, 0x73, 0x00, 0x00, 0x00, 0x72, 0x00, 0x6d, 0x00,
          0x52, 0x41, 0x4d, 0x44, 0x55, 0x4d, 0x7e, 0x31, 0x53, 0x52, 0x4d, 0x20, 0x00, 0x00, 0x00, 0x00,
          0x17, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x4f, 0x26, 0x15, 0x00, 0x00, 0x02, 0x00,
          0x41, 0x74, 0x00, 0x73, 0x00, 0x74, 0x00, 0x66, 0x00, 0x69, 0x00, 0x0f, 0x00, 0xf9, 0x6c, 0x00,
          0x65, 0x00, 0x30, 0x00, 0x31, 0x00, 0x2e, 0x00, 0x64, 0x00, 0x00, 0x00, 0x61, 0x00, 0x74, 0x00,
          0x54, 0x53, 0x54, 0x46, 0x49, 0x4c, 0x7e, 0x31, 0x44, 0x41, 0x54, 0x20, 0x00, 0x00, 0x00, 0x00,
          0x17, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x4f, 0x2a, 0x15, 0x00, 0x00, 0x02, 0x00,
  [511] = 0x00
};

static unsigned char registeryDat[] = {
  [  0] = 0x2f, 0x52, 0x4f, 0x4d, 0x53, 0x2f, 0x67, 0x65, 0x6f, 0x64, 0x61, 0x73, 0x68, 0x2e, 0x67, 0x62,
          0x61, 0x00, 0x43, 0x6f, 0x75, 0x6e, 0x74, 0x72, 0x79, 0x2e, 0x67, 0x62, 0x61, 0x00, 0x20, 0x53,
          0x61, 0x70, 0x70, 0x68, 0x69, 0x72, 0x65, 0x2e, 0x67, 0x62, 0x61, 0x00, 0x68, 0x65, 0x20, 0x53,
          0x69, 0x74, 0x68, 0x2e, 0x67, 0x62, 0x61, 0x00, 0x6f, 0x72, 0x64, 0x73, 0x2e, 0x67, 0x62, 0x61,
  [320] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xac, 0x8f, 0x02, 0x00, 0x30, 0x31, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x02, 0x01, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x74, 0x08, 0x37, 0x32,
  [511] = 0x00
};

static unsigned char emptySector[512] = {0};

static bool
Disk_ReadStatic(unsigned int index, void *buffer) {
  static unsigned char *sectors[] = {
    [0x00000] = bootRecord,
    [0x02000] = superBlock,
    [0x02492] = NULL, // first FAT
    [0x06000] = rootDir,
    [0x06040] = gbasysDir,
    [0x06a00] = sysDir,
    [0x5a740] = registeryDat,
  };

  unsigned char *source = emptySector;
  if (sectors[index] != NULL) {
    source = sectors[index];
  }

  unsigned char *target = buffer;
  for (int i = 0; i < 512; i++) {
    target[i] = source[i];
  }

  return true;
}

test(Initialize_ShouldReadDiskInfo) {
  // arrange
  Disk disk = {0};
  DiskInfo *info = &disk.info;

  // act
  bool result = Disk_Initialize(&disk, Disk_ReadStatic);

  // assert
  assert(result == true);
  assert(info->bytesPerSector == 512);
  assert(info->sectorsPerCluster == 64);
  assert(info->sectorsPerFAT == 7607);
  assert(info->rootDirCluster == 2);
  assert(info->firstFATSector == 9362);
  assert(info->firstDataSector == 24576);
}

test(ReadDirectory_ShouldReturnExpectedEntries) {
  // arrange
  Disk disk = {0};
  Disk_Initialize(&disk, Disk_ReadStatic);

  char *path[] = { NULL };
  Disk_OpenDirectory(&disk, path);

  DiskEntry entries[] = {
    { .type = DISK_ENTRY_DIRECTORY, .name = { "GBASYS     " }, .fileSize = 0, .startCluster = 3 },
    { .type = DISK_ENTRY_DIRECTORY, .name = { "ROMS       " }, .fileSize = 0, .startCluster = 45 },
    { .type = DISK_ENTRY_FILE, .name = { "BOOT-V~1UPD" }, .fileSize = 28672, .startCluster = 5406 },
  };

  // act
  for (int i = 0; i < length(entries); i++) {
    DiskEntry entry = {0};
    Disk_ReadDirectory(&disk, &entry);

    // assert
    assert(entry.type == entries[i].type);
    assert(entry.fileSize == entries[i].fileSize);
    assert(entry.startCluster == entries[i].startCluster);
    assert(DiskEntry_NameEquals(&entry, entries[i].name));
  }
}

test(ReadDirectory_ShouldReturnFalseWhenAllEntriesAreRead) {
  // arrange
  Disk disk = {0};
  Disk_Initialize(&disk, Disk_ReadStatic);

  char *path[] = { NULL };
  Disk_OpenDirectory(&disk, path);

  int count = 0;
  DiskEntry entry = {0};

  // act
  while (Disk_ReadDirectory(&disk, &entry)) {
    count++;
  }

  // assert
  assert(count == 3);
}

test(OpenDirectory_ShouldReturnTrueIfSubdirectoriesExist) {
  // arrange
  Disk disk = {0};
  Disk_Initialize(&disk, Disk_ReadStatic);

  char *path[] = {
    "GBASYS     ",
    "SYS        ",
    NULL
  };

  // act
  bool result = Disk_OpenDirectory(&disk, path);

  // assert
  assert(result == true);
}

test(OpenDirectory_ShouldReturnFalseIfSubdirectoryDoesNotExist) {
  // arrange
  Disk disk = {0};
  Disk_Initialize(&disk, Disk_ReadStatic);

  char *path[] = {
    "GBASYS     ",
    "SYS        ",
    "NONEXISTENT",
    NULL
  };

  // act
  bool result = Disk_OpenDirectory(&disk, path);

  // assert
  assert(result == false);
}

test(OpenDirectory_ShouldReturnFalseIfSubdirectoryIsFile) {
  // arrange
  Disk disk = {0};
  Disk_Initialize(&disk, Disk_ReadStatic);

  char *path[] = {
    "GBASYS     ",
    "SYS        ",
    "RECENT  DAT",
    NULL
  };

  // act
  bool result = Disk_OpenDirectory(&disk, path);

  // assert
  assert(result == false);
}

test(OpenDirectory_ShouldOpenDifferentSubdirectoriesAfterOneAnother) {
  // arrange
  Disk disk = {0};
  Disk_Initialize(&disk, Disk_ReadStatic);

  char *path1[] = { NULL };
  char *path2[] = { "GBASYS     ", NULL };

  int count = 0;
  DiskEntry entry = {0};

  // act
  if (Disk_OpenDirectory(&disk, path1)) {
    while (Disk_ReadDirectory(&disk, &entry)) {
      count++;
    }
  }

  if (Disk_OpenDirectory(&disk, path2)) {
    while (Disk_ReadDirectory(&disk, &entry)) {
      count++;
    }
  }

  // assert
  assert(count == 10);
}

test(ReadFile_ShouldFillBufferWithContentsOfFileAndReturnTrue) {
  // arrange
  Disk disk = {0};
  Disk_Initialize(&disk, Disk_ReadStatic);

  char *path[] = {
    "GBASYS     ",
    "SYS        ",
    NULL
  };

  Disk_OpenDirectory(&disk, path);

  DiskEntry entry = {0};
  while (Disk_ReadDirectory(&disk, &entry)) {
    if (DiskEntry_NameEquals(&entry, "REGIST~1DAT")) {
      break;
    }
  }

  unsigned char buffer[1024] = {0};
  unsigned int length = sizeof(buffer);

  // act
  bool result = Disk_ReadFile(&disk, &entry, buffer, length);

  // assert
  assert(result == true);

  for (int i = 0; i < entry.fileSize; i++) {
    assert(buffer[i] == registeryDat[i]);
  }
}

test(OpenFile_ShouldReturnReaderAndReadFileByteByByte) {
  // arrange
  Disk disk = {0};
  Disk_Initialize(&disk, Disk_ReadStatic);

  char *path[] = {
    "GBASYS     ",
    "SYS        ",
    NULL
  };

  Disk_OpenDirectory(&disk, path);

  DiskEntry entry = {0};
  while (Disk_ReadDirectory(&disk, &entry)) {
    if (DiskEntry_NameEquals(&entry, "REGIST~1DAT")) {
      break;
    }
  }

  // act
  DataSource *source = Disk_OpenFile(&disk, &entry);
  Reader *reader = DataSource_AsReader(source);

  // assert
  assert(reader != NULL);

  for (int i = 0; i < entry.fileSize; i++) {
    int byte = Reader_Read(reader);
    assert(byte == registeryDat[i]);
  }

  int byte = Reader_Read(reader);
  assert(byte == -1);
}

test(NormalizePath_ShouldReturnExpectedNormalizedPath) {
  // arrange
  const char *pathname = "/GBASYS/sys/registery.dat";

  char segments[4][12] = {0};
  char *path[] = {
    segments[0],
    segments[1],
    segments[2],
    segments[3],
    NULL,
  };

  const char *expected[] = {
    "GBASYS     ",
    "SYS        ",
    "REGIST~1DAT",
  };

  // act
  bool result = DiskEntry_NormalizePath(pathname, path);

  // assert
  assert(result == true);
  assert(path[3] == NULL);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 12; j++) {
      assert(expected[i][j] == path[i][j]);
    }
  }
}

test(NormalizePath_ShouldReturnExpectedNormalizedPathForRootDirectory) {
  // arrange
  const char *pathname = "/";

  char segments[3][12] = {0};
  char *path[] = {
    segments[0],
    segments[1],
    segments[2],
    NULL,
  };

  // act
  bool result = DiskEntry_NormalizePath(pathname, path);

  // assert
  assert(result == true);
  assert(path[0] == NULL);
  assert(path[1] == segments[1]);
  assert(path[2] == segments[2]);
  assert(path[3] == NULL);
}

test(NormalizePath_ShouldReturnFalseWhenPathnameContainsInvalidCharacters) {
  // arrange
  char segments[4][12] = {0};
  const char *pathnames[] = {
    "/GBASYS/sys/reg:stery.dat",
    "///",
    "/GBASYS/sys/.dat",
    "/GBASYS/./romcfg",
  };

  for (int i = 0; i < length(pathnames); i++) {
    char *path[] = {
      segments[0],
      segments[1],
      segments[2],
      segments[3],
      NULL,
    };

    // act
    bool result = DiskEntry_NormalizePath(pathnames[i], path);

    // assert
    assert(result == false);
  }
}

test(NormalizePath_ShouldReturnFalseWhenPathnameExceedsCapacityOfProvidedPath) {
  // arrange
  const char *pathname = "/GBASYS/sys/regstery.dat";

  char segments[2][12] = {0};
  char *path[] = {
    segments[0],
    segments[1],
    NULL,
  };

  // act
  bool result = DiskEntry_NormalizePath(pathname, path);

  // assert
  assert(result == false);
}

test(DirnameOf_ShouldReturnExpectedFilenameAndAlterPath) {
  // arrange
  char *folder1 = "GBASYS     ";
  char *folder2 = "SYS        ";
  char *file    = "RECENT  DAT";

  char *path[] = { folder1, folder2, file, NULL };

  // act
  char *filename = DiskEntry_DirnameOf(path);

  // assert
  assert(filename == file);
  assert(path[0] == folder1);
  assert(path[1] == folder2);
  assert(path[2] == NULL);
  assert(path[3] == NULL);
}

test(DirnameOf_ShouldReturnNullForRootDirectory) {
  // arrange
  char *path[] = { NULL };

  // act
  char *filename = DiskEntry_DirnameOf(path);

  // assert
  assert(filename == NULL);
  assert(path[0] == NULL);
}

suite(
  Initialize_ShouldReadDiskInfo,
  ReadDirectory_ShouldReturnExpectedEntries,
  ReadDirectory_ShouldReturnFalseWhenAllEntriesAreRead,
  OpenDirectory_ShouldReturnTrueIfSubdirectoriesExist,
  OpenDirectory_ShouldReturnFalseIfSubdirectoryDoesNotExist,
  OpenDirectory_ShouldReturnFalseIfSubdirectoryIsFile,
  OpenDirectory_ShouldOpenDifferentSubdirectoriesAfterOneAnother,
  ReadFile_ShouldFillBufferWithContentsOfFileAndReturnTrue,
  OpenFile_ShouldReturnReaderAndReadFileByteByByte,
  NormalizePath_ShouldReturnExpectedNormalizedPath,
  NormalizePath_ShouldReturnExpectedNormalizedPathForRootDirectory,
  NormalizePath_ShouldReturnFalseWhenPathnameContainsInvalidCharacters,
  NormalizePath_ShouldReturnFalseWhenPathnameExceedsCapacityOfProvidedPath,
  DirnameOf_ShouldReturnExpectedFilenameAndAlterPath,
  DirnameOf_ShouldReturnNullForRootDirectory);
