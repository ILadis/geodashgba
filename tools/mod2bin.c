
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

  DataSource *output = File_From(&(File) {0}, stdout);
  Writer *writer = DataSource_AsWriter(output);

  Writer_Printf(writer, "\n");
  Writer_Printf(writer, "#include <sound.h>\n");
  Writer_Printf(writer, "\n");

  for (unsigned int channel = 0; channel < 4; channel++) {
    SoundTrack *module = ModuleSoundTrack_From(&(ModuleSoundTrack) {0}, source, channel);
    SoundTrack *binv1 = Binv1SoundTrack_From(&(Binv1SoundTrack) {0}, target);

    Reader *reader = DataSource_AsReader(target);
    Reader_SeekTo(reader, 0);

    if (!SoundTrack_Convert(module, binv1)) {
      Logger_PrintLine(log, "Could not convert module track, size exceeded?");
      return 1;
    }

    unsigned int length = Reader_GetPosition(reader);

    Logger_Print(log, "Converted module track #");
    Logger_PrintHex8(log, channel);
    Logger_Print(log, " to binv1 successful, wrote 0x");
    Logger_PrintHex16(log, length);
    Logger_Print(log, " bytes");
    Logger_PrintNewline(log);

    Writer_Printf(writer, "const unsigned char %s%ldBinv1Track[] = {\n", "test", channel);

    unsigned int index = 0;
    Reader_SeekTo(reader, 0);

    while (index < length) {
      int byte = Reader_Read(reader);

      Writer_Printf(writer,   index % 8 == 0 ? "  " : "");
      Writer_Printf(writer, "0x%02X,", byte);
      Writer_Printf(writer, ++index % 8 == 0 ? "\n" : " ");
    }

    Writer_Printf(writer, "};\n");
    Writer_Printf(writer, "\n");
  }

  for (unsigned int sample = 0; sample < 32; sample++) {
    SoundSampler *sampler = ModuleSoundSampler_From(&(ModuleSoundSampler) {0}, source, sample);
    SoundSampler *binv1 = Binv1SoundSampler_ConvertFrom(&(Binv1SoundSampler) {0}, target, sampler);

    unsigned char volume = SoundSampler_GetVolume(sampler);
    if (volume == 0) break;

    Reader *reader = DataSource_AsReader(target);

    unsigned int length = Reader_GetPosition(reader);

    Logger_Print(log, "Converted module sample #");
    Logger_PrintHex8(log, sample);
    Logger_Print(log, " to binv1 successful, wrote 0x");
    Logger_PrintHex16(log, length);
    Logger_Print(log, " bytes");
    Logger_PrintNewline(log);

    Writer_Printf(writer, "const unsigned char %s%ldBinv1Sample[] = {\n", "test", sample);

    unsigned int index = 0;
    Reader_SeekTo(reader, 0);

    while (index < length) {
      int byte = Reader_Read(reader);

      Writer_Printf(writer,   index % 8 == 0 ? "  " : "");
      Writer_Printf(writer, "0x%02X,", byte);
      Writer_Printf(writer, ++index % 8 == 0 ? "\n" : " ");
    }

    Writer_Printf(writer, "\n");
    Writer_Printf(writer, "};\n");
    Writer_Printf(writer, "\n");
  }
}
