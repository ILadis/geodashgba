
#define NOGBA

#include <io.h>
#include <sound.h>

/* Produce and play sound files with:
 *   tools/mksnd > tools/play.snd
 *   ffplay -f s8 -ar 8192 -ac 1 tools/play.snd
 */
int main(int argc, char **argv) {
  char notes[] = "C D E F G G A A A A G G A A A A G G ";

  const int rate = 13; // = 8192 sample rate
  const int count = sizeof(notes) / 2; // number of notes
//const int size = (sizeof(char) << 7) - 1; // sample size (1 byte)
  const int length = (1 << rate) * count; // buffer size

  int samples[sizeof(int) * length];

  SoundChannel *channel = NoteSoundChannel_Create(&(NoteSoundChannel) {0}, notes, rate);
  SoundChannel_Fill(channel, samples, length);

  DataSource *output = File_From(&(File) {0}, stdout);
  Writer *writer = DataSource_AsWriter(output);

  for (int i = 0; i < length; i++) {
    int byte = samples[i];
    Writer_Write(writer, byte);
  }

  // TODO close file handle (also in error cases)

  return 0;
}
