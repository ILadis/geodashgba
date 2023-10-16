
#include <sound.h>

// frequences of octave zero as 24.8 fixed point integers
static const int Octave[] = {
  [NOTE_C]      = 16.3516 * (1 << 8),
  [NOTE_CSHARP] = 17.3239 * (1 << 8),
  [NOTE_D]      = 18.3540 * (1 << 8),
  [NOTE_DSHARP] = 19.4454 * (1 << 8),
  [NOTE_E]      = 20.6017 * (1 << 8),
  [NOTE_F]      = 21.8268 * (1 << 8),
  [NOTE_FSHARP] = 23.1247 * (1 << 8),
  [NOTE_G]      = 24.4997 * (1 << 8),
  [NOTE_GSHARP] = 25.9565 * (1 << 8),
  [NOTE_A]      = 27.5000 * (1 << 8),
  [NOTE_ASHARP] = 29.1352 * (1 << 8),
  [NOTE_B]      = 30.8677 * (1 << 8),
};

static int
NoteSample_Get(
    void *self,
    int index)
{
  NoteSample *sample = self;
  Note note = sample->note;

  if (index > sample->length || note > length(Octave)) {
    return 0;
  }

  const int pi = 256;
  const int octave = sample->octave;

  /* Note: multiplying frequency (24.8 fixed point) with index (for example a 19.13 fixed
   * point for when using 8kHz as the sample rate) results in a 11.21 fixed point number.
   * This is why we have to shift by 21 to get the alpha/integer part that is used to get
   * the sin-value.
   */
  int frequency = Octave[note] * (1 << octave);
  int alpha = (pi * frequency * index) >> (8 + sample->rate);

  // returns a 24.8 fixed point integer
  int value = Math_sin(alpha);

  // convert to sample size of 8 bits (range -127..+127)
  return (value * 127) >> 8;
}

static inline NoteSample*
NoteSample_FromSamples(union Samples *samples) {
  NoteSample *sample = &samples->note;
  sample->super.self = sample;
  sample->super.Get = NoteSample_Get;
  return sample;
}

static inline bool
NoteSoundChannel_NextSample(NoteSoundChannel *channel) {
  int index = channel->track.index;
  char *notes = channel->track.notes;

  char symbol = notes[index];
  if (symbol == '\0' || symbol < 'A' || symbol > 'Z') {
    return false;
  }

  enum Note note = (symbol - 'A') * 2;

  int octave = 4;
  int dotted = 0;
  int length = channel->tempo;
  bool shorten = false;

  while (true) {
    char modifier = notes[++index];
    if (modifier == ' ' || modifier == '|') {
      continue;
    }

    switch (modifier) {
      case '^': note += 1; continue;
      case '_': note -= 1; continue;
      case '`': octave += 1; continue;
      case ',': octave -= 1; continue;
      case '/': shorten = true; continue;
      case '2': length = shorten ? length >> 1 : length << 1; continue;
      case '4': length = shorten ? length >> 2 : length << 2; continue;
      case '8': length = shorten ? length >> 3 : length << 3; continue;
      case '.': length += length >> ++dotted; continue;
    }

    break;
  }

  NoteSample *sample = NoteSample_FromSamples(&channel->samples);
  sample->note = note;
  sample->octave = octave;
  sample->length = length;
  sample->rate = channel->rate;

  channel->track.index = index;
  channel->sample = &sample->super;
  channel->length = length;
  channel->position = 0;

  return true;
}

SoundChannel*
NoteSoundChannel_Create(
    NoteSoundChannel *channel,
    char *notes,
    int rate)
{
  int NoteSoundChannel_Fill(void *channel, int *buffer, int size);

  channel->super.self = channel;
  channel->super.Fill = NoteSoundChannel_Fill;

  channel->track.notes = notes;
  channel->track.index = 0;
  channel->rate = rate;
  channel->tempo = 2500;
  channel->position = 0;
  channel->length = 0;

  NoteSoundChannel_NextSample(channel);

  return &channel->super;
}

int
NoteSoundChannel_Fill(
    void *self,
    int *buffer,
    int size)
{
  NoteSoundChannel *channel = self;
  Sample *sample = channel->sample;

  int length = channel->length;
  int offset = 0;

  if (channel->position >= length) {
      return 0; // end of track
  }

  while (offset < size) {
    int value = Sample_Get(sample, channel->position++);
    buffer[offset++] += value;

    if (channel->position < length) {
      continue;
    }

    // advance to next sample (note or pause)
    if (!NoteSoundChannel_NextSample(channel)) {
      break;
    }
  }

  return offset;
}
