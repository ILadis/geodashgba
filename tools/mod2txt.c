
#define NOGBA

#include <sound.h>
#include <log.h>

int main() {
  Logger *log = Logger_GetInstance();

  DataSource *input = File_From(&(File) {0}, stdin);
  DataSource *output = File_From(&(File) {0}, stdout);

  Reader *reader = DataSource_AsReader(input);
  Writer *writer = DataSource_AsWriter(output);

  for (unsigned int sample = 0; sample < 32; sample++) {
    SoundSampler *sampler = ModuleSoundSampler_From(&(ModuleSoundSampler) {0}, reader, sample);

    if (sampler == NULL) {
      Logger_PrintLine(log, "Could not parse mod sample.");
      return 1;
    }

    unsigned char volume = SoundSampler_GetVolume(sampler);
    if (volume == 0) continue;

    unsigned int data[1024] = {0};

    DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
    Writer *buffer = DataSource_AsWriter(source);

    Binv1SoundSampler_To(sampler, buffer);

    Reader *reader = DataSource_AsReader(source);
    unsigned int length = Reader_GetPosition(reader);

    for (unsigned int i = 0; i < length; i++) {
      Writer_Printf(writer, "%d, ", data[i]);
    }
    Writer_Printf(writer, "\n");
  }

  for (unsigned int channel = 0; channel < 4; channel++) {
    SoundTrack *track = ModuleSoundTrack_From(&(ModuleSoundTrack) {0}, reader, channel);

    if (track == NULL) {
      Logger_PrintLine(log, "Could not parse mod track.");
      return 1;
    }

    AsciiSoundTrack_To(track, writer, '|');
    Writer_Printf(writer, "\n");
  }
}
