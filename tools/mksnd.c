
#define NOGBA

#include <io.h>
#include <sound.h>

static void song1(SoundPlayer *player) {
  static const char notes1[] = "C`. B/2 A G | A F E D | G Z B2 | C Z2 G/2 G/2 | G G Z G^/2 G^/2 | A A Z G/2 G/2 | G C` B A | B2. G/2 G/2 | G G Z G^/2 G^/2 | A A Z G/2 G/2 | G F` E` D^` | E`2. G/2 G/2 | G G Z G^/2 G^/2 | A A Z G/2 G/2 | G C` B A | B2 Z D`/2 D`/2 | D` C` Z C`/2 C`/2 | C` B Z B/2 B/2 | A C` B  A  | G G/2 G/2 G G | E`2. B  | C` C` G E | F. G/2 G G | G4 | F`2. B | C` B A F | E. G/2 G G | G4 | E`2. G | B_ B_ B_ B_ | A. D`/2 D` D` | D^`2. D^` | E` C`/2 B/2 A G | F E D C | A Z B Z | C` C`/2 C`/2 C` Z | C2 F C^ | D2 F2 | E4 | B_4 | C2 E C^ | D2 E2 | C4 | A4 | A` Z A` A` | A`  Z A` A` | B_`2 B_`. B_`/2 | B_`4 | B_`2 B_`. B_`/2 | B_`4 | A`2 A`. A`/2 | A`4 | C`2 F` C^` | D`2 F`2 | E`4 | B_`4 | C`2 E` C^` | D`2  E`2 | C`4 | A`4 | Z A`2 A` | A`2 A` A`  | C``2. F` | B_`2 F`2  | F`2. C` | G`2. C | F` Z E` D`/2 E`/2 | F` F`/2 F`/2 F` Z |";
  static const char notes2[] = "G.  Z2 Z/2  | F Z2 Z  | E Z Z2 | Z Z2 E/2 E/2 | E E Z E/2  E/2  | E E Z E/2 E/2 | E E  E E | F2. F/2 F/2 | F F Z F/2  F/2  | F F Z2        | Z A  G  F^  | G2.  E/2 E/2 | E E Z E/2  E/2  | E E Z E/2 E/2 | E E  E E | G2 Z F^/2 F^/2 | F^ F^ Z F/2  F/2  | F  E Z Z       | Z Z  F^ F^ | Z4            | C`2. G^ | A  G  E C | B. F/2 F F | F4 | B2.  G | A  G F D | C. E/2 E E | E4 | C`2. Z | G  G  G  G  | F. A/2  A  A  | A2.   A   | C` Z2.          | Z4      | F Z G Z | G  G/2  G/2  G  Z | Z4      | Z4    | Z4 | Z4  | Z4      | Z4    | Z4 | Z4 | F` Z E` D` | C^` Z D` F` | E`2  C^`. D`/2  | D`4  | E`2  C^`. D`/2  | D`4  | D`2 B.  C`/2 | C`4 | A2  A  A   | A2  A2  | B_4 | E`4  | B_2 B_ B_  | B_2  B_2 | A4  | C`4 | Z F`2 F` | F`2 E` E_` | F`2.  D` | G`2  D_`2 | D`2. Z  | E`2. Z | C` Z2.            | Z  C`/2 C`/2 C` Z |";
  static const char notes3[] = "E`. Z2 Z/2  | Z4      | Z4     | Z4           | Z4              | Z4            | Z4       | Z4          | Z4              | Z4            | Z4          | Z4           | Z4              | Z4            | Z4       | Z4             | Z4                | Z4             | Z4         | Z4            | Z4      | Z4        | Z4         | Z4 | Z4     | Z4       | Z4         | Z4 | Z4     | Z4          | Z4            | Z4        | G  Z2.          | Z4      | Z2  F Z | E  E/2  E/2  E  Z | Z4      | Z4    | Z4 | Z4  | Z4      | Z4    | Z4 | Z4 | Z4         | Z4          | Z4              | Z4   | Z4              | Z4   | Z4           | Z4  | Z4         | Z4      | Z4  | Z4   | Z4         | Z4       | Z4  | Z4  | Z4       | Z4         | D`2.  Z  | D`2  Z2   | C`2. Z  | B_2. Z | A  Z2.            | Z  A/2  A/2  A  Z |";

  static NoteSoundChannel channel1 = {0};
  static NoteSoundChannel channel2 = {0};
  static NoteSoundChannel channel3 = {0};

  SoundPlayer_AddChannel(player, NoteSoundChannel_Create(&channel1, notes1, 13)); // 2^13 = 8192 sample rate
  SoundPlayer_AddChannel(player, NoteSoundChannel_Create(&channel2, notes2, 13));
  SoundPlayer_AddChannel(player, NoteSoundChannel_Create(&channel3, notes3, 13));
}

static void song2(SoundPlayer *player) {
  static const char notes1[] = "E` | E` B/2  C`/2 D`/2 E`/4 D`/4 C`/2 B/2  | A A/2 C`/2 E`  D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z            | Z/2 D`/2 F`/2 A` G`/2 F`/2 | E`. C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z | E` B/2  C`/2 D` C`/2 B/2 | A A/2 C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z            | Z/2 D`/2 F`/2 A` G`/2 F`/2 | E`. C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z | E2 C2  | D2  B,2  | C2  A,2 | G^,2 B,2  | E2 C2  | D2  B,2  | C  E A A | G^4 | E` B/2  C`/2 D` C`/2 B/2 | A A/2 C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z            | Z/2 D`/2 F`/2 A` G`/2 F`/2 | E`. C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A2 |";
  static const char notes2[] = "B  | Z  G^/2 Z/2  Z              A/2  G^/2 | E Z        Z2            | G^ G^/2 A/2  B  Z  | Z2 Z/2 A/2 B/2 C`/2 | Z/2 F/2  A/2  C` B/2  A/2  | G.  E/2  G  F/2  E/2  | G^ G^/2 A/2  B  Z  | Z4       | Z  G^/2 Z/2  Z  A/2  G/2 | E Z        Z2           | G^ G^/2 A/2  B  Z  | Z2 Z/2 A/2 B/2 C`/2 | Z/2 F/2  A/2  C` B/2  A/2  | G.  E/2  G  F/2  E/2  | G^ G^/2 A/2  B  Z  | Z4       | C2 A,2 | B,2 G^,2 | A,2 D,2 | D,2  G^,2 | C2 A,2 | B,2 G^,2 | A, C E E | E4  | Z  G^/2 Z/2  Z  A/2  G/2 | E Z        Z2           | G^ G^/2 A/2  B  Z  | Z2 Z/2 A/2 B/2 C`/2 | Z/2 F/2  A/2  C` B/2  A/2  | G.  E/2  G  F/2  E/2  | G^ G^/2 A/2  B  Z  | Z4      |";

  static NoteSoundChannel channel1 = {0};
  static NoteSoundChannel channel2 = {0};

  SoundPlayer_AddChannel(player, NoteSoundChannel_Create(&channel1, notes1, 13)); // 2^13 = 8192 sample rate
  SoundPlayer_AddChannel(player, NoteSoundChannel_Create(&channel2, notes2, 13));
}

/* Produce and play sound files with:
 *   tools/mksnd > tools/play.snd
 *   ffplay -f s8 -ar 8192 -ac 1 tools/play.snd
 */
int main() {
  SoundPlayer *player = SoundPlayer_GetInstance();
  SoundPlayer_Enable(player);

  player->frequency = 1<<13;
  song2(player);

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
