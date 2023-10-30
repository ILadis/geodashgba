
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

  // get sector of first partition
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

  DiskInfo *info = &disk->info;

  unsigned int bytesPerSector = Disk_BufferGetU16At(disk, 0x000B);
  unsigned int sectorsPerCluster = Disk_BufferGetU8At(disk, 0x000D);

  info->sectorSize = Math_log2(bytesPerSector);
  info->clusterSize = Math_log2(sectorsPerCluster);

  unsigned short sectorsPerFAT = Disk_BufferGetU16At(disk, 0x0024);

  unsigned short reservedSectors = Disk_BufferGetU16At(disk, 0x000E);
  unsigned short numberOfFATs = Disk_BufferGetU8At(disk, 0x0010);

  unsigned int rootDirCluster = Disk_BufferGetU32At(disk, 0x002C);
  unsigned int firstDataSector = offset + reservedSectors + numberOfFATs * sectorsPerFAT;
  unsigned int firstFATSector = offset + reservedSectors;

  info->rootDirCluster = rootDirCluster;
  info->firstFATSector = firstFATSector;
  info->firstDataSector = firstDataSector;

  return true;
}

static inline unsigned int
Disk_GetSectorOfCluster(Disk *disk, unsigned int cluster) {
  return disk->info.firstDataSector + (cluster - 2) * (1 << disk->info.clusterSize);
}

static inline unsigned int
Disk_GetNextCluster(Disk *disk, unsigned int cluster) {
  DiskInfo *info = &disk->info;

  unsigned int fatOffset = cluster * 4;
  unsigned int fatSector = disk->info.firstFATSector + (fatOffset >> info->sectorSize);
  unsigned int fatEntry = Math_mod2(fatOffset, info->sectorSize);

  if (!Disk_BufferFill(disk, fatSector)) {
    return Disk_BadCluster;
  }

  unsigned int next = Disk_BufferGetU32At(disk, fatEntry) & Disk_ClusterMask;
  return next;
}

static inline bool
Disk_SectorIsAlignedWithCluster(Disk *disk, unsigned int sector) {
  DiskInfo *info = &disk->info;
  const unsigned int sectorsPerCluster = DiskInfo_SectorsPerCluster(info);
  return (sector & (sectorsPerCluster - 1)) == 0;
}

static int
DiskReader_ReadNext(void *self);

static int
DiskReader_ReadAdvance(void *self) {
  Disk *disk = self;
  DiskReader *reader = &disk->reader;

  unsigned int sector = reader->sector + 1;
  unsigned int cluster = reader->cluster;

  if (Disk_SectorIsAlignedWithCluster(disk, sector)) {
    cluster = Disk_GetNextCluster(disk, cluster);
    switch (cluster) {
      case Disk_BadCluster:  return -1;
      case Disk_ClusterMask: return -1;
    }
    sector = Disk_GetSectorOfCluster(disk, cluster);
  }

  if (!Disk_BufferFill(disk, sector)) {
    return -1;
  }

  reader->position = 0;
  reader->sector = sector;
  reader->cluster = cluster;

  DataSource *source = &disk->source;
  source->reader.Read = DiskReader_ReadNext;

  return DiskReader_ReadNext(self);
}

static int
DiskReader_ReadNext(void *self) {
  Disk *disk = self;
  DiskReader *reader = &disk->reader;
  DiskInfo *info = &disk->info;

  if (reader->size <= 0) {
    return -1;
  }

  unsigned int position = reader->position++;
  const unsigned int bytesPerSector = DiskInfo_BytesPerSector(info);

  if (reader->position >= bytesPerSector) {
    DataSource *source = &disk->source;
    source->reader.Read = DiskReader_ReadAdvance;
  }

  reader->size--;
  return Disk_BufferGetU8At(disk, position);
}

static bool
DiskReader_SeekTo(
    void *self,
    unsigned int position)
{
  Disk *disk = self;
  DiskInfo *info = &disk->info;
  DiskEntry *entry = &disk->entry;

  if (position > entry->fileSize) {
    return false;
  }

  unsigned int cluster = entry->startCluster;
  const int seeks = position >> (info->clusterSize + info->sectorSize);

  for (int i = 0; i < seeks; i++) {
    cluster = Disk_GetNextCluster(disk, cluster);
    switch (cluster) {
      case Disk_BadCluster:  return false;
      case Disk_ClusterMask: return false;
    }
  }

  unsigned int sector = Disk_GetSectorOfCluster(disk, cluster);

  const int delta = (position >> info->sectorSize) - (1 << info->clusterSize) * seeks;
  const int offset = Math_mod2(position, info->sectorSize);

  sector += delta;
  if (disk->reader.sector != sector) {
    if (!Disk_BufferFill(disk, sector)) {
      return false;
    }
  }

  DiskReader *reader = &disk->reader;
  reader->cluster = cluster;
  reader->size = entry->fileSize - position;
  reader->sector = sector;
  reader->position = offset;

  DataSource *source = &disk->source;
  source->reader.Read = DiskReader_ReadNext;

  return true;
}

static int
DiskReader_InitializeRead(void *self) {
  Disk *disk = self;
  DiskReader *reader = &disk->reader;
  DiskEntry *entry = &disk->entry;

  unsigned int cluster = entry->startCluster;
  unsigned int sector = Disk_GetSectorOfCluster(disk, cluster);

  if (!Disk_BufferFill(disk, sector)) {
    return -1;
  }

  reader->cluster = cluster;
  reader->sector = sector;

  return DiskReader_ReadNext(self);
}

static unsigned int
DiskReader_GetLength(void *self) {
  Disk *disk = self;
  DiskEntry *entry = &disk->entry;

  return entry->fileSize;
}

DataSource*
Disk_OpenFile(
    Disk *disk,
    const DiskEntry *entry)
{
  DataSource *source = &disk->source;

  source->reader.self = disk;
  source->reader.Read = DiskReader_InitializeRead;
  source->reader.SeekTo = DiskReader_SeekTo;
  source->reader.GetLength = DiskReader_GetLength;

  disk->entry = *entry;

  DiskReader *reader = &disk->reader;
  reader->size = entry->fileSize;
  reader->cluster = 0;
  reader->sector = 0;
  reader->position = 0;

  return source;
}

bool
Disk_OpenDirectory(
    Disk *disk,
    char *path[])
{
  DiskInfo *info = &disk->info;
  DiskEntry root = {
    .startCluster = info->rootDirCluster,
    .type = DISK_ENTRY_DIRECTORY,
    .fileSize = 0xFFFFFFFF,
  };

  int index = 0;
  Disk_OpenFile(disk, &root);

  while (path[index] != NULL) {
    char *name = path[index++];

    DiskEntry entry = {0};
    do {
      if (!Disk_ReadDirectory(disk, &entry)) {
        return false;
      }
    } while (!DiskEntry_NameEquals(&entry, name));

    if (entry.type != DISK_ENTRY_DIRECTORY) {
      return false;
    }

    Disk_OpenFile(disk, &entry);
  }

  return true;
}

bool
Disk_ReadDirectory(
    Disk *disk,
    DiskEntry *entry)
{
  Reader *reader = DataSource_AsReader(&disk->source);
  if (disk->entry.type != DISK_ENTRY_DIRECTORY) {
    return false;
  }

  unsigned int position = 0;
  unsigned char flags = 0;

  next: {
    for (int i = 0; i < Disk_EntrySize; i++) {
      int byte = Reader_Read(reader);
      if (byte < 0) return false;
    }

    position = disk->reader.position - Disk_EntrySize;
    flags = Disk_BufferGetU8At(disk, position);
    // check end of entries marker or deleted entry
    if (flags == 0x00 || flags == 0xE5) {
      return false;
    }

    flags = Disk_BufferGetU8At(disk, position + 11);
    // skip hidden and system files
    if (flags & 0b00110) {
      goto next;
    }
  }

  const int length = strlen(entry->name);
  for (int i = 0; i < length; i++) {
    entry->name[i] = Disk_BufferGetU8At(disk, position + i);
  }

  entry->name[length] = '\0';
  entry->type = flags & 0b10000 ? DISK_ENTRY_DIRECTORY : DISK_ENTRY_FILE;
  entry->startCluster = Disk_BufferGetU16At(disk, position + 26);
  entry->fileSize = Disk_BufferGetU32At(disk, position + 28);

  if (entry->type == DISK_ENTRY_DIRECTORY) {
    entry->fileSize = 0xFFFFFFFF;
  }

  return true;
}

bool
DiskEntry_NameEquals(
    const DiskEntry *entry,
    char *name)
{
  const int length = strlen(entry->name);
  for (int i = 0; i < length; i++) {
    if (name[i] == '\0' || name[i] != entry->name[i]) {
      return false;
    }
  }

  return true;
}

static inline bool
DiskEntry_NormalizePathSegment(
    const char **segment,
    char **normalized,
    const char *delimeters,
    int limit, bool shrink)
{
  static char charmap[] = {
    // special characters:
    [0x21] = '!', [0x23] = '#', [0x24] = '$', [0x25] = '%',
    [0x26] = '&', [0x27] = '\'',[0x28] = '(', [0x29] = ')',
    [0x2D] = '-', [0x40] = '@', [0x60] = '`', [0x7B] = '{',
    [0x7D] = '}', [0x7E] = '~',

    // numbers/digits:
    [0x30] = '0', [0x31] = '1', [0x32] = '2', [0x33] = '3',
    [0x34] = '4', [0x35] = '5', [0x36] = '6', [0x37] = '7',
    [0x38] = '8', [0x39] = '9',

    // upper case characters:
    [0x41] = 'A', [0x42] = 'B', [0x43] = 'C', [0x44] = 'D',
    [0x45] = 'E', [0x46] = 'F', [0x47] = 'G', [0x48] = 'H',
    [0x49] = 'I', [0x4A] = 'J', [0x4B] = 'K', [0x4C] = 'L',
    [0x4D] = 'M', [0x4E] = 'N', [0x4F] = 'O', [0x50] = 'P',
    [0x51] = 'Q', [0x52] = 'R', [0x53] = 'S', [0x54] = 'T',
    [0x55] = 'U', [0x56] = 'V', [0x57] = 'W', [0x58] = 'X',
    [0x59] = 'Y', [0x5A] = 'Z',

    // invalid characters:
    [0x22] = '\0', [0x2A] ='\0', [0x2B] = '\0', [0x2C] = '\0',
    [0x2E] = '\0', [0x2F] ='\0', [0x3A] = '\0', [0x3B] = '\0',
    [0x3C] = '\0', [0x3D] ='\0', [0x3E] = '\0', [0x3F] = '\0',
    [0x5B] = '\0', [0x5C] ='\0', [0x5D] = '\0', [0x7C] = '\0',

    // lower case are mapped to upper case:
    [0x61] = 'A', [0x62] = 'B', [0x63] = 'C', [0x64] = 'D',
    [0x65] = 'E', [0x66] = 'F', [0x67] = 'G', [0x68] = 'H',
    [0x69] = 'I', [0x6A] = 'J', [0x6B] = 'K', [0x6C] = 'L',
    [0x6D] = 'M', [0x6E] = 'N', [0x6F] = 'O', [0x70] = 'P',
    [0x71] = 'Q', [0x72] = 'R', [0x73] = 'S', [0x74] = 'T',
    [0x75] = 'U', [0x76] = 'V', [0x77] = 'W', [0x78] = 'X',
    [0x79] = 'Y', [0x7A] = 'Z',
  };

  int index = 0;

  do {
    char byte = (*segment)[index];
    if (byte >= (char) length(charmap)) {
      return false; // invalid character
    }

    int i = 0;
    do {
      if (delimeters[i] == byte) {
        goto done;
      }
    } while (delimeters[i++] != '\0');

    char glyph = charmap[(int) byte];
    if (glyph == '\0') {
      return false; // invalid character
    }

    if (index < limit) {
      (*normalized)[index] = glyph;
    }

    index++;
  } while (true);

done:
  // empty file names are not allowed
  if (shrink && index == 0) {
    return false;
  }

  // fill remaining characters with spaces
  for (int i = index; i < limit; i++) {
    (*normalized)[i] = ' ';
  }

  if (index >= limit && shrink) {
    (*normalized)[limit - 2] = '~';
    (*normalized)[limit - 1] = '1';
  }

  *segment += index;
  *normalized += limit;

  return true;
}

bool
DiskEntry_NormalizePath(
    const char *pathname,
    char *path[])
{
  int index = 0;

  do {
    char *segment = path[index];
    if (segment == NULL) {
      return false;
    }

    if (*pathname == '/' && *(++pathname) == '\0') {
      break;
    }

    if (!DiskEntry_NormalizePathSegment(&pathname, &segment, "./", 8, true)) {
      return false;
    }

    if (*pathname == '.') {
      pathname++;
    }

    if (!DiskEntry_NormalizePathSegment(&pathname, &segment, "/", 3, false)) {
      return false;
    }

    index++;
  } while (*pathname != '\0');

  path[index] = NULL;

  return true;
}

char*
DiskEntry_DirnameOf(char *path[]) {
  int index = 0;

  while (path[index] != NULL) {
    char *current = path[index];
    char *next = path[index + 1];

    if (next == NULL) {
      path[index] = NULL;
      return current;
    }

    index++;
  }

  return NULL;
}
