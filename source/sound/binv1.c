
#include <sound.h>

static const Tone*
Binv1SoundTrack_NextTone(void *self) {
  Binv1SoundTrack *track = self;

  unsigned int index = track->position;
  if (index >= track->length) {
    return NULL;
  }

  const Tone *tone = &track->tones[index];
  track->position++;

  return tone;
}

static bool
Binv1SoundTrack_SeekTo(
    void *self,
    unsigned int position)
{
  Binv1SoundTrack *track = self;

  if (position > track->length) {
    return false;
  }

  track->position = position;
  return true;
}

SoundTrack*
Binv1SoundTrack_From(
    Binv1SoundTrack *track,
    const Tone *tones,
    unsigned int length)
{
  track->base.self = track;
  track->position = 0;
  track->length = length;
  track->tones = tones;

  track->base.Next = Binv1SoundTrack_NextTone;
  track->base.SeekTo = Binv1SoundTrack_SeekTo;

  return &track->base;
}

static int
Binv1SoundSampler_GetSample(void *self, unsigned int index) {
  Binv1SoundSampler *sampler = self;

  unsigned int length = sampler->length;
  unsigned int position = index & (length - 1); // length is mask for index (modulo of some power of 2)

  int sample = sampler->samples[position];
  return sample;
}

static unsigned int
Binv1SoundSampler_GetFrequency(void *self, const Tone *tone) {
  Binv1SoundSampler *sampler = self;

  unsigned int frequency = sampler->frequencies[tone->note];
  return frequency << tone->octave;
}

static unsigned char
Binv1SoundSampler_GetVolume(void *self) {
  Binv1SoundSampler *sampler = self;

  unsigned char volume = sampler->volume;
  return volume;
}

static inline void
Binv1SoundSampler_ConvertVolumeAndFrequencies(
    Binv1SoundSampler *sampler,
    const SoundSampler *other)
{
  static unsigned int frequencies[NOTE_COUNT] = {0};

  unsigned char volume = SoundSampler_GetVolume(other);
  sampler->volume = volume;

  for (enum Note note = 0; note < NOTE_COUNT; note++) {
    Tone tone = {
      .octave = 0,
      .note = note,
    };

    unsigned int frequency = SoundSampler_GetFrequency(other, &tone);
    frequencies[note] = frequency;
  }

  sampler->frequencies = frequencies;
}

static inline void
Binv1SoundSampler_ConvertSamples(
    Binv1SoundSampler *sampler,
    const SoundSampler *other)
{
  static int samples[1024] = {0};
  for (unsigned int index = 0; index < length(samples); index++) {
    int sample = SoundSampler_GetSample(other, index);
    samples[index] = sample;
  }

  unsigned int length = length(samples);
  unsigned int shortest = length;

next:
  while (length > 0) {
    // check if samples repeat with this length
    for (unsigned int index = 0; index < length(samples) - length; index++) {
      if (samples[index] != samples[index + length]) {
        length >>= 1;
        goto next;
      }
    }

    shortest = length;
    length >>= 1;
  }

  sampler->length = shortest;
  sampler->samples = samples;
}

SoundSampler*
Binv1SoundSampler_ConvertFrom(
    Binv1SoundSampler *sampler,
    const SoundSampler *other)
{
  sampler->base.self = sampler;

  sampler->base.Get  = Binv1SoundSampler_GetSample;
  sampler->base.Frequency = Binv1SoundSampler_GetFrequency;
  sampler->base.Volume = Binv1SoundSampler_GetVolume;

  Binv1SoundSampler_ConvertVolumeAndFrequencies(sampler, other);
  Binv1SoundSampler_ConvertSamples(sampler, other);

  return &sampler->base;
}

unsigned int
Binv1SoundSampler_To(
    Binv1SoundSampler *sampler,
    unsigned int *data)
{
  unsigned int index = 0;

  data[index++] = sampler->volume;
  data[index++] = sampler->length;

  for (unsigned int i = 0; i < NOTE_COUNT; i++) {
    data[index++] = sampler->frequencies[i];
  }

  for (unsigned int i = 0; i < sampler->length; i++) {
    data[index++] = sampler->samples[i];
  }

  return index;
}

SoundSampler*
Binv1SoundSampler_From(
    Binv1SoundSampler *sampler,
    const unsigned int *data)
{
  sampler->base.self = sampler;

  sampler->base.Get  = Binv1SoundSampler_GetSample;
  sampler->base.Frequency = Binv1SoundSampler_GetFrequency;
  sampler->base.Volume = Binv1SoundSampler_GetVolume;

  sampler->volume = data[0];
  sampler->length = data[1];

  sampler->frequencies = &data[2];
  sampler->samples = (const int *) &data[2 + NOTE_COUNT];

  return &sampler->base;
}

