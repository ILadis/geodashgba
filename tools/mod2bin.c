
#define NOGBA

#include <sound.h>
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
    Logger_PrintLine(log, "Could not open mod file.");
    return 1;
  }

  SoundTrack *module = ModuleSoundTrack_From(&(ModuleSoundTrack) {0}, source, 0);
  SoundTrack *binv1 = Binv1SoundTrack_From(&(Binv1SoundTrack) {0}, target);

  if (!SoundTrack_Convert(module, binv1)) {
    Logger_PrintLine(log, "Could not convert module track, size exceeded?");
    return 1;
  }

  unsigned int length = buffer.position;

  Logger_Print(log, "Converted module track to binv1 successful, wrote 0x");
  Logger_PrintHex32(log, length);
  Logger_Print(log, " bytes");
  Logger_PrintNewline(log);

  DataSource *output = File_From(&(File) {0}, stdout);
  Reader *reader = DataSource_AsReader(target);
  Writer *writer = DataSource_AsWriter(output);

  Writer_Printf(writer, "\n");
  Writer_Printf(writer, "#include <sound.h>\n");
  Writer_Printf(writer, "\n");
  Writer_Printf(writer, "const unsigned char %sBinv1Track[] = {\n", "test");

  Reader_SeekTo(reader, 0);
  for (unsigned int index = 0; index < length;) {
    int byte = Reader_Read(reader);
    Writer_Printf(writer,   index % 8 == 0 ? "  " : "");
    Writer_Printf(writer, "0x%02X,", byte);
    Writer_Printf(writer, ++index % 8 == 0 ? "\n" : " ");
  }

  Writer_Printf(writer, "};\n");
}
