
#define NOGBA

#include <game/level.h>
#include <log.h>

static char data[1024 * 1024 * 5] = {0};

int main(int argc, char **argv) {
  Logger *log = Logger_GetInstance();

  if (argc != 2) {
    Logger_PrintLine(log, "Invalid number of arguments given.");
    return 1;
  }

  Buffer buffer;

  DataSource *source = File_Open(&(File) {0}, argv[1], "rb+");
  DataSource *target = Buffer_From(&buffer, data, length(data));

  if (source == NULL) {
    Logger_PrintLine(log, "Could not open level file.");
    return 1;
  }

  Level *ascii = AsciiLevel_From(&(AsciiLevel) {0}, source);
  Level *binv1 = Binv1Level_From(&(Binv1Level) {0}, target);

  int chunks = Level_Convert(ascii, binv1);
  int length = buffer.position;

  if (chunks < 0) {
    Logger_PrintLine(log, "Could not add chunk to level, size exceeded?");
  } else {
    Logger_Print(log, "Wrote 0x");
    Logger_PrintHex16(log, chunks);
    Logger_Print(log, " chunk(s) with length 0x");
    Logger_PrintHex16(log, length);
    Logger_PrintNewline(log);
  }

  DataSource *output = File_From(&(File) {0}, stdout);
  Reader *reader = DataSource_AsReader(target);
  Writer *writer = DataSource_AsWriter(output);

  Reader_SeekTo(reader, 0);
  while (length-- > 0) {
    int byte = Reader_Read(reader);
    Writer_Write(writer, byte);
  }
}
