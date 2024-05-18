
#define NOGBA

#include <io.h>
#include <sound.h>

/* Produce and play sound files with:
 *   tools/mod2snd < song.mod > tools/play.snd
 *   ffplay -f s8 -ar 16384 -ac 1 tools/play.snd
 */
int main() {
  DataSource *input = File_From(&(File) {0}, stdin);
  Reader *reader = DataSource_AsReader(input);

  SoundPlayer *player = SoundPlayer_GetInstance();
  SoundPlayer_SetFrequency(player, 16384);

  static ModuleSoundSampler samplers[32] = {0};
  for (unsigned int i = 0; i < length(samplers); i++) {
    ModuleSoundSampler_FromReader(&samplers[i], reader, i);
  }

  static ModuleSoundTrack tracks[4] = {0};
  for (unsigned int i = 0; i < length(tracks); i++) {
    ModuleSoundTrack_FromReader(&tracks[i], reader, i);
  }

  static SoundChannel channels[4] = {0};
  for (unsigned int i = 0; i < length(channels); i++) {
    SoundPlayer_AddChannel(player, &channels[i]);

    SoundChannel_SetSpeed(&channels[i], 6);
    SoundChannel_SetTempo(&channels[i], 125);
    SoundChannel_AssignTrack(&channels[i], &tracks[i].base);

    for (unsigned int j = 0; j < length(samplers); j++) {
      SoundChannel_AddSampler(&channels[i], &samplers[j].base);
    }
  }

  DataSource *output = File_From(&(File) {0}, stdout);
  Writer *writer = DataSource_AsWriter(output);

  while (SoundPlayer_MixChannels(player)) {
    for (unsigned int i = 0; i < player->size; i++) {
      Writer_Write(writer, (unsigned char) player->active[i]);
    }

    SoundPlayer_VSync(player);
  }

  // TODO close file handle (also in error cases)

  return 0;
}
