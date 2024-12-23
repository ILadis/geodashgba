
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

    // TODO consider adding sample index to output
    AsciiSoundSampler_To(sampler, writer);
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
