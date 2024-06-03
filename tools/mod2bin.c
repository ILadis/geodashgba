
#define NOGBA

#include <sound.h>
#include <log.h>

int main(int argc, char **argv) {
  Logger *log = Logger_GetInstance();

  if (argc != 2) {
    Logger_PrintLine(log, "Invalid number of arguments given.");
    return 1;
  }

  DataSource *source = File_From(&(File) {0}, stdin);

  if (source == NULL) {
    Logger_PrintLine(log, "Could not open mod file.");
    return 1;
  }

  const char *prefix = argv[1];

  DataSource *output = File_From(&(File) {0}, stdout);
  Writer *writer = DataSource_AsWriter(output);

  Writer_Printf(writer, "\n");
  Writer_Printf(writer, "#include <sound.h>\n");
  Writer_Printf(writer, "\n");

  for (unsigned int channel = 0; channel < 4; channel++) {
    SoundTrack *track = ModuleSoundTrack_From(&(ModuleSoundTrack) {0}, source, channel);

    Writer_Printf(writer, "const Tone %sTones%ld[] = {\n", prefix, channel);
    while (true) {
      const Tone *tone = SoundTrack_NextTone(track);
      if (tone == NULL) break;

      Writer_Printf(writer, "  {\n");
      Writer_Printf(writer, "    .note = %u,\n", tone->note);
      Writer_Printf(writer, "    .effect = { .type = %u, .param = %u },\n", tone->effect.type, tone->effect.param);
      Writer_Printf(writer, "    .octave = %u,\n", tone->octave);
      Writer_Printf(writer, "    .sample = %d,\n", tone->sample);
      Writer_Printf(writer, "    .ticks = %u,\n", tone->ticks);
      Writer_Printf(writer, "  },\n");
    }
    Writer_Printf(writer, "};\n");
    Writer_Printf(writer, "\n");
  }

  for (unsigned int sample = 0; sample < 32; sample++) {
    SoundSampler *module = ModuleSoundSampler_From(&(ModuleSoundSampler) {0}, source, sample);

    Binv1SoundSampler sampler = {0};
    Binv1SoundSampler_ConvertFrom(&sampler, module);

    if (sampler.volume == 0) break;

    unsigned int data[1024] = {0};
    unsigned int length = Binv1SoundSampler_To(&sampler, data);

    Writer_Printf(writer, "const unsigned int %sSamples%u[] = { ", prefix, sample);
    for (unsigned int i = 0; i < length; i++) {
      Writer_Printf(writer, "%d, ", data[i]);
    }
    Writer_Printf(writer, "};\n");
    Writer_Printf(writer, "\n");
  }
}
