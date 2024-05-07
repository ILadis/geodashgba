
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
SineSoundSampler_GetSample(
    unused void *self,
    unsigned int index)
{
  // returns a 1.8 fixed point integer
  int value = Math_sin(index);

  // convert to sample size of 8 bits (range -127..+127)
  return (value * 127) >> 8;
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

const SoundSampler*
SineSoundSampler_GetInstance() {
  static SoundSampler sampler = {
    .self = NULL,
    .Get = SineSoundSampler_GetSample,
    .Frequency = SineSoundSampler_GetFrequency,
  };

  return &sampler;
}
