
#define NOGBA

#include <io.h>
#include <sound.h>

static void song1(SoundPlayer *player, const SoundSampler *sampler) {
  static const char notes1[] = "Z/2 E/2. F/4 E/4 E_/4 E/2 C`/2 | B2. | Z/2 E/2. F/4 E/4 E_/4 E/2 B/2 | A2. | Z/2 A/2 B/4 A/4 A_/4 A/4 E`/2  G`/2 | F` Z/2 F`/2 F`/2 Z/2 | F` G` F` | F` E`2 | Z Z/2 C`/2 D`/2 E`/2 | E`. D`/2 D` | D` Z/2 D`/2 E`/2 D`/2 | D`. C`/2 C` | C`2 A/2 B/2 | C`.  D`/2 C | B E` B | A2. | A/2 E/2. F/4 E/4 E_/4 E/2 C`/2 | B2. | Z/2 E/2. F/4 E/4 E_/4 E/2 B/2 | A2. | Z/2 A/2 B/4 A/4 A_/4 A/4 E`/2  G`/2 | F` Z/2 F`/2 F`/2 Z/2 | F` G` F` | F` E`2 | Z Z/2 C`/2 D`/2 E`/2 | E`. D`/2 D` | D` Z/2 D`/2 E`/2 D`/2 | D`. C`/2 C` | C`2 A/2 B/2 | C`. D`/2 C` | B E` C` | A2. | A/2 Z/2 Z/2 A/2 A/4 B/4 C`^/4 D`/4 | E`.  A/2 A/2 Z/2 | A C`^ E`  | G` F`. E`/2  | F`2. | D`. G/2 G/2 Z/2 | G B D` | F` E` E`_/2 Z/2` | E`2. | A. B/2 C`/2 Z/2 | G. Z/2 C`/2 F`/2 | F` E` B | C`. D`/2 E` | A. A`/2 A`/2 Z/2 | A` G`^ A` | G`^. B`/2 G`^/2 Z/2 | E`2.    | A.  E/2  E/2  Z/2 | Z A/2 B/2 C`^/2 D`/2 | E`2. | E`. E`/2 F`/2 G`/2 | A`2. | A`2 G`/4 A`/4 G`/4 F`/4 | G`2 F`/4 G`/4 F`/2 | E`2. | D`. E`/2 F`/2 Z/2 | F` E` D` | C`. Z/2 D` | E` A` C`` | B`. Z/2 A`/2 Z/2 | G`^. F`^/2 G`^/2 Z/2 | A`2.  | A`2.   | A`2.  | E`2. |";
  static const char notes2[] = "Z2 Z                           | Z2. | Z2 Z                          | Z2. | Z2                       C`^/2 E`/2 | D` Z/2 D`/2 D`/2 Z/2 | D` E` D` | D` C`2 | Z2 Z                 | C`. B_/2 B_ | B_ Z2                 | B.  A/2  A  | A2  Z       | Z2 Z        | Z2 Z   | Z2. | Z2 Z                           | Z2. | Z2 Z                          | Z2. | Z2                       C`^/2 E`/2 | D` Z/2 D`/2 D`/2 Z/2 | D` E` D` | D` C`2 | Z2 Z                 | C`. B_/2 B_ | B_ Z2                 | B.  A/2  A  | A2  Z       | Z2 Z        | Z2 Z    | Z2. | Z2 Z                               | C`^. Z/2 Z       | Z A   C`^ | E` D`. C`^/2 | D`2. | B.  Z/2 Z       | Z G B  | D` C` C`/2  Z/2  | C`2. | F. G/2 A/2  Z/2 | E. Z/2 A/2  D`/2 | D` C` A | A`. B/2  C` | Z2 Z             | E2     E  | D`.  Z/2  Z         | E F^ G^ | C^. C^/2 C^/2 Z/2 | Z2 Z                 | Z2.  | A.  Z/2  Z         | F`2. | Z2 Z                    | Z2 Z               | Z2.  | B.  Z         Z/2 | D` Z  Z  | A.  Z/2 A_ | G  E` E`  | F`. Z/2 F`/2 Z/2 | E`.  Z/2   Z         | Z E`2 | Z F`^2 | Z E`2 | B2.  |";

  static AsciiSoundTrack sound1 = {0};
  SoundTrack *track1 = AsciiSoundTrack_FromNotes(&sound1, notes1);

  static AsciiSoundTrack sound2 = {0};
  SoundTrack *track2 = AsciiSoundTrack_FromNotes(&sound2, notes2);

  static ModuleSoundChannel module1 = {0};
  SoundChannel *channel1 = ModuleSoundChannel_ForTrack(&module1, track1);

  ModuleSoundChannel_AddSampler(&module1, sampler);
  SoundPlayer_AddChannel(player, channel1);

  static ModuleSoundChannel module2 = {0};
  SoundChannel *channel2 = ModuleSoundChannel_ForTrack(&module2, track2);

  ModuleSoundChannel_AddSampler(&module2, sampler);
  SoundPlayer_AddChannel(player, channel2);
}

unused static void song2(SoundPlayer *player, const SoundSampler *sampler) {
  static const char notes1[] = "E` | E` B/2  C`/2 D`/2 E`/4 D`/4 C`/2 B/2  | A A/2 C`/2 E`  D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z            | Z/2 D`/2 F`/2 A` G`/2 F`/2 | E`. C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z | E` B/2  C`/2 D` C`/2 B/2 | A A/2 C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z            | Z/2 D`/2 F`/2 A` G`/2 F`/2 | E`. C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z | E2 C2  | D2  B,2  | C2  A,2 | G^,2 B,2  | E2 C2  | D2  B,2  | C  E A A | G^4 | E` B/2  C`/2 D` C`/2 B/2 | A A/2 C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z            | Z/2 D`/2 F`/2 A` G`/2 F`/2 | E`. C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A2 |";
  static const char notes2[] = "B  | Z  G^/2 Z/2  Z              A/2  G^/2 | E Z        Z2            | G^ G^/2 A/2  B  Z  | Z2 Z/2 A/2 B/2 C`/2 | Z/2 F/2  A/2  C` B/2  A/2  | G.  E/2  G  F/2  E/2  | G^ G^/2 A/2  B  Z  | Z4       | Z  G^/2 Z/2  Z  A/2  G/2 | E Z        Z2           | G^ G^/2 A/2  B  Z  | Z2 Z/2 A/2 B/2 C`/2 | Z/2 F/2  A/2  C` B/2  A/2  | G.  E/2  G  F/2  E/2  | G^ G^/2 A/2  B  Z  | Z4       | C2 A,2 | B,2 G^,2 | A,2 D,2 | D,2  G^,2 | C2 A,2 | B,2 G^,2 | A, C E E | E4  | Z  G^/2 Z/2  Z  A/2  G/2 | E Z        Z2           | G^ G^/2 A/2  B  Z  | Z2 Z/2 A/2 B/2 C`/2 | Z/2 F/2  A/2  C` B/2  A/2  | G.  E/2  G  F/2  E/2  | G^ G^/2 A/2  B  Z  | Z4      |";

  static AsciiSoundTrack sound1 = {0};
  SoundTrack *track1 = AsciiSoundTrack_FromNotes(&sound1, notes1);

  static AsciiSoundTrack sound2 = {0};
  SoundTrack *track2 = AsciiSoundTrack_FromNotes(&sound2, notes2);

  static ModuleSoundChannel module1 = {0};
  SoundChannel *channel1 = ModuleSoundChannel_ForTrack(&module1, track1);

  ModuleSoundChannel_AddSampler(&module1, sampler);
  SoundPlayer_AddChannel(player, channel1);

  static ModuleSoundChannel module2 = {0};
  SoundChannel *channel2 = ModuleSoundChannel_ForTrack(&module2, track2);

  ModuleSoundChannel_AddSampler(&module2, sampler);
  SoundPlayer_AddChannel(player, channel2);
}

/* Produce and play sound files with:
 *   tools/mksnd > tools/play.snd
 *   ffplay -f s8 -ar 8192 -ac 1 tools/play.snd
 */
int main() {
  const SoundSampler *sampler = SineSoundSampler_GetInstance();

  SoundPlayer *player = SoundPlayer_GetInstance();
  SoundPlayer_SetFrequency(player, 8192);

  song1(player, sampler);

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
