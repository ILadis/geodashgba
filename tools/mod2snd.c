
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

  ModuleTrack module = {0};
  ModuleTrack_FromReader(&module, reader);

  SoundPlayer *player = SoundPlayer_GetInstance();
  SoundPlayer_SetFrequency(player, 16384);

  static SoundChannel channels[4] = {{0}};

  for (unsigned int i = 0; i < module.numChannels; i++) {
    SoundPlayer_AddChannel(player, &channels[i]);

    SoundTrack *track = ModuleTrack_GetSoundTrack(&module, i);
    SoundSampler *sampler = ModuleTrack_GetSoundSampler(&module, i);

    SoundChannel_SetTrackAndSampler(&channels[i], track, sampler);
    SoundChannel_SetTempo(&channels[i], player->frequency / 20); // 1/20th of a second per full note
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
