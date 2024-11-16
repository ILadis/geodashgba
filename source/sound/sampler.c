
#include <sound.h>

// frequences of octave zero as 24.8 fixed point integers
static const unsigned int Octave[] = {
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

static inline unsigned int
Tone_GetFrequency(const Tone *tone) {
  return (tone->note > length(Octave)) ? 0 : Octave[tone->note] * (1 << tone->octave);
}

static int
NullSoundSampler_GetSample() {
  return 0;
}

static int*
NullSoundSampler_FillBuffer(
    unused void *self, int *buffer,
    unsigned int *position,
    unsigned int increment,
    unused unsigned char volume,
    unsigned int size)
{
  *position += size * increment;
  return &buffer[size];
}

static unsigned int
NullSoundSampler_GetLength() {
  return 0;
}

static unsigned int
NullSoundSampler_GetFrequency() {
  return 0;
}

static unsigned char
NullSoundSampler_GetVolume() {
  return 1 << SOUND_VOLUME_PRECISION;
}

const SoundSampler*
NullSoundSampler_GetInstance() {
  static SoundSampler sampler = {
    .self = NULL,
    .Get = NullSoundSampler_GetSample,
    .Fill = NullSoundSampler_FillBuffer,
    .Length = NullSoundSampler_GetLength,
    .Frequency = NullSoundSampler_GetFrequency,
    .Volume = NullSoundSampler_GetVolume,
  };

  return &sampler;
}

static inline int
SineSoundSampler_GetSample(
    unused void *self,
    unsigned int index)
{
  // returns a 1.8 fixed point integer
  int value = Math_sin(index);

  // convert to sample size of 8 bits (range -127..+127)
  return (value * 127) >> 8;
}

static int*
SineSoundSampler_FillBuffer(
    void *self, int *buffer,
    unsigned int *position,
    unsigned int increment,
    unsigned char volume,
    unsigned int size)
{
  unsigned int $position = *position;

  while (size-- > 0) {
    const unsigned int index = ($position) >> SOUND_CHANNEL_PRECISION;
    int value = SineSoundSampler_GetSample(self, index);

    *buffer++ += (value * volume) >> SOUND_VOLUME_PRECISION;
    $position += increment;
  }

  *position = $position;

  return buffer;
}

static unsigned int
SineSoundSampler_GetFrequency(
    unused void *self,
    const Tone *tone)
{
  const int pi2 = 256;

  // returns a 24.8 fixed point integer
  unsigned int frequency = Tone_GetFrequency(tone);
  unsigned int alpha = (pi2 * frequency) >> 8;

  return alpha;
}

static unsigned int
SineSoundSampler_GetLength(unused void *self) {
  return 0;
}

static unsigned char
SineSoundSampler_GetVolume(unused void *self) {
  return 1 << SOUND_VOLUME_PRECISION;
}

const SoundSampler*
SineSoundSampler_GetInstance() {
  static SoundSampler sampler = {
    .self = NULL,
    .Get  = SineSoundSampler_GetSample,
    .Fill = SineSoundSampler_FillBuffer,
    .Length = SineSoundSampler_GetLength,
    .Frequency = SineSoundSampler_GetFrequency,
    .Volume = SineSoundSampler_GetVolume,
  };

  return &sampler;
}
