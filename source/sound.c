
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

Sample*
NoteSample_Create(
    NoteSample *sample,
    Note note,
    int rate)
{
  int NoteSample_Get(void *sample, int index);

  sample->super.self = sample;
  sample->super.Get = NoteSample_Get;

  sample->note = note;
  sample->rate = rate;
  sample->length = 1 << rate;

  return &sample->super;
}

Sample*
NoteSample_NextFrom(
    NoteSample *sample,
    char *notes,
    int rate)
{
  enum Note note = Note_FromText(notes);
  return NoteSample_Create(sample, note, rate);
}

int
NoteSample_Get(
    void *self,
    int index)
{
  NoteSample *sample = self;
  Note note = sample->note;

  if (index > sample->length) {
    return 0;
  }

  const int pi = 256;
  const int octave = 4; // TODO currently hard coded

  /* Note: multiplying frequency (24.8 fixed point) with index (19.13 fixed point for
   * 8k sample rate) results in a 11.21 fixed point number. This is why we have to shift
   * by 21 to get the alpha/integer part that is used to get the sin-value.
   */
  int frequency = Octave[note] * (1 << octave);
  int alpha = (pi * frequency * index) >> (8 + sample->rate);

  // returns a 24.8 fixed point integer
  int value = Math_sin(alpha);

  // convert to sample size of 8 bits (range -127..+127)
  return (value * 127) >> 8;
}

SoundChannel*
NoteSoundChannel_Create(
    NoteSoundChannel *channel,
    char *notes,
    int rate)
{
  void NoteSoundChannel_Fill(void *channel, int *buffer, int size);

  channel->super.self = channel;
  channel->super.Fill = NoteSoundChannel_Fill;

  channel->notes = notes;
  channel->position = 0;
  channel->volume = 255;

  NoteSample_NextFrom(&channel->sample, notes, rate);

  return &channel->super;
}

void
NoteSoundChannel_Fill(
    void *self,
    int *buffer,
    int size)
{
  NoteSoundChannel *channel = self;
  NoteSample *sample = &channel->sample;

  char *notes = &channel->notes[0];
  int length = channel->sample.length;

  while (size > 0) {
    int index = channel->position;
    int value = NoteSample_Get(sample, index);

    *buffer = value;

    channel->position += 1;
    buffer += 1;
    size -= 1;

    // advance to next note
    if (channel->position >= length) {
      notes += 2;

      if (notes[0] != '\0') {
        NoteSample_NextFrom(sample, notes, sample->rate);

        channel->position = 0;
        channel->notes = notes;
      }
    }
  }
}
