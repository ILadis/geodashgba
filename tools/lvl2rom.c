
#define NOGBA

#include <game/collection.h>
#include <log.h>

static char data[1024 * 1024 * 5] = {0};

int main(int argc, char **argv) {
  Logger *log = Logger_GetInstance();

  if(argc != 2) {
    Logger_PrintLine(log, "Invalid number of arguments given.\n");
    return 1;
  }

  Buffer buffer;
  DataSource *source = Buffer_From(&buffer, data, length(data));
  DataSource *target = File_Open(&(File) {0}, argv[1], "rb+");

  if (target == NULL) {
    Logger_PrintLine(log, "Could not open rom file.");
    return 1;
  }

  DataSource *input = File_From(&(File) {0}, stdin);
  buffer.length = DataSource_CopyFrom(source, input);

  Reader *reader = DataSource_AsReader(target);
  Collection *collection = Collection_DefineWithUsableSpace(300 * 1024);

  int attempts = 5;
  while (attempts-- > 0) {
    if (Collection_ReadFrom(collection, reader)) {
      break;
    }
  }

  if (attempts <= 0) {
    Logger_PrintLine(log, "Could not find collection in rom file.");
    return 1;
  }

  int count = Collection_GetLevelCount(collection);
  int length = collection->length;

  Logger_Print(log, "Found level collection of size 0x");
  Logger_PrintHex16(log, length);
  Logger_Print(log, ", containing 0x");
  Logger_PrintHex16(log, count);
  Logger_PrintLine(log, " levels");

  Binv1Level level;
  Binv1Level_From(&level, source);

  if (!Collection_AddLevel(collection, &level)) {
    Logger_PrintLine(log, "Could not add level to collection.");
    return 1;
  }

  // TODO find better way to restore position
  Reader_SeekTo(reader, 0);
  while (attempts++ < 5) {
    if (!Collection_FindSignature(reader)) {
      break;
    }
  }

  Writer *writer = DataSource_AsWriter(target);
  Collection_WriteTo(collection, writer);

  // TODO close file handle (also in error cases)
}
