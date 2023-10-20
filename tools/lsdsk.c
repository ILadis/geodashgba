
#define NOGBA

#include <io.h>
#include <disk.h>
#include <math.h>
#include <log.h>

static DataSource *source = NULL;

bool read(unsigned int sector, void *buffer) {
  Reader *reader = DataSource_AsReader(source);
  if (!Reader_SeekTo(reader, sector * 512)) {
    return false;
  }

  unsigned char *target = buffer;
  for (int i = 0; i < 512; i++) {
    int byte = Reader_Read(reader);
    if (byte < 0) {
      return false;
    }

    target[i] = (unsigned char) byte;
  }

  return true;
}

int main(int argc, char **argv) {
  Logger *log = Logger_GetInstance();

  if (argc < 3) {
    Logger_PrintLine(log, "Invalid number of arguments given.");
    return 1;
  }

  DataSource *output = File_From(&(File) {0}, stdout);
  Writer *writer = DataSource_AsWriter(output);

  char segments[31][12] = {0};
  char *path[32] = {0};
  for (int i = 0; i < length(segments); i++) {
    path[i] = segments[i];
  }

  if (!DiskEntry_NormalizePath(argv[2], path)) {
    Logger_PrintLine(log, "Invalid path given.");
    return 1;
  }

  source = File_Open(&(File) {0}, argv[1], "rb+");
  if (source == NULL) {
    Logger_PrintLine(log, "Could not open disk file.");
    return 1;
  }

  Disk disk = {0};
  if (!Disk_Initialize(&disk, read)) {
    Logger_PrintLine(log, "Could not initialize disk.");
    return 1;
  }

  char *filename = DiskEntry_DirnameOf(path);
  if (filename == NULL) {
    Logger_PrintLine(log, "Invalid path: no filename.");
    return 1;
  }

  if (!Disk_OpenDirectory(&disk, path)) {
    Logger_PrintLine(log, "Could not open directory.");
    return 1;
  }

  DiskEntry entry = {0};
  do {
    if (!Disk_ReadDirectory(&disk, &entry)) {
      Logger_PrintLine(log, "File not found.");
      return 1;
    }
  } while (!DiskEntry_NameEquals(&entry, filename));

  DataSource *source = Disk_OpenFile(&disk, &entry);
  if (source == NULL) {
    Logger_PrintLine(log, "Could not open file.");
    return 1;
  }

  Reader *reader = DataSource_AsReader(source);
  if (argc > 3) {
    extern int atoi(const char *string);
    int position = atoi(argv[3]);
    Reader_SeekTo(reader, position);
  }

  for (int i = 0; i < entry.fileSize; i++) {
    int byte = Reader_Read(reader);
    Writer_Write(writer, (unsigned char) byte);
  }

  return 0;
}