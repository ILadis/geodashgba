
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

  const unsigned int length = sampler->length;
  const unsigned int position = index & (length - 1); // length is mask for index (modulo of some power of 2)

  int sample = sampler->samples[position];
  return sample;
}

static int*
Binv1SoundSampler_FillBuffer(
    void *self, int *buffer,
    unsigned int *position,
    unsigned int increment,
    unsigned char volume,
    unsigned int size)
{
  Binv1SoundSampler *sampler = self;

  const unsigned int length = sampler->length - 1;

  while (size-- > 0) {
    const unsigned int index = (*position >> SOUND_CHANNEL_PRECISION) & length;
    int value = sampler->samples[index];

    *buffer++ += (value * volume) >> SOUND_VOLUME_PRECISION;
    *position += increment;
  }

  return buffer;
}

static unsigned int
Binv1SoundSampler_GetLength(void *self) {
  Binv1SoundSampler *sampler = self;
  return sampler->length;
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

SoundSampler*
Binv1SoundSampler_From(
    Binv1SoundSampler *sampler,
    const unsigned int *data)
{
  sampler->base.self = sampler;
  sampler->reader = NULL;

  sampler->base.Get  = Binv1SoundSampler_GetSample;
  sampler->base.Fill = Binv1SoundSampler_FillBuffer;
  sampler->base.Length = Binv1SoundSampler_GetLength;
  sampler->base.Frequency = Binv1SoundSampler_GetFrequency;
  sampler->base.Volume = Binv1SoundSampler_GetVolume;

  sampler->volume = data[0];
  sampler->frequencies = &data[1];

  sampler->length = data[1 + NOTE_COUNT];
  sampler->samples = (const int *) &data[2 + NOTE_COUNT];

  return &sampler->base;
}

static unsigned int
Binv1SoundSampler_GetLengthFromReader(void *self) {
  Binv1SoundSampler *sampler = self;

  const Reader *reader = sampler->reader;
  Reader_SeekTo(reader, 52);

  unsigned int length = 0;
  Reader_ReadInt32(reader, &length);

  return length;
}

static int
Binv1SoundSampler_GetSampleFromReader(void *self, unsigned int index) {
  Binv1SoundSampler *sampler = self;

  const unsigned int length = Binv1SoundSampler_GetLengthFromReader(self);
  unsigned int position = 56 + (index % length) * 4;

  const Reader *reader = sampler->reader;
  Reader_SeekTo(reader, position);

  int sample = 0;
  Reader_ReadInt32(reader, &sample);

  return sample;
}

static int*
Binv1SoundSampler_FillBufferFromReader(
    void *self, int *buffer,
    unsigned int *position,
    unsigned int increment,
    unsigned char volume,
    unsigned int size)
{
  Binv1SoundSampler *sampler = self;
  return SoundSampler_SlowFillBuffer(&sampler->base, buffer, position, increment, volume, size);
}

static unsigned int
Binv1SoundSampler_GetFrequencyFromReader(void *self, const Tone *tone) {
  Binv1SoundSampler *sampler = self;

  const Reader *reader = sampler->reader;
  Reader_SeekTo(reader, 4 + tone->note * 4);

  unsigned int frequency = 0;
  Reader_ReadUInt32(reader, &frequency);

  return frequency << tone->octave;
}

static unsigned char
Binv1SoundSampler_GetVolumeFromReader(void *self) {
  Binv1SoundSampler *sampler = self;

  const Reader *reader = sampler->reader;
  Reader_SeekTo(reader, 0);

  unsigned int volume = 0;
  Reader_ReadUInt32(reader, &volume);

  return (unsigned char) volume;
}

SoundSampler*
Binv1SoundSampler_FromReader(
    Binv1SoundSampler *sampler,
    const Reader *reader)
{
  sampler->base.self = sampler;
  sampler->reader = reader;

  sampler->base.Get  = Binv1SoundSampler_GetSampleFromReader;
  sampler->base.Fill = Binv1SoundSampler_FillBufferFromReader;
  sampler->base.Length = Binv1SoundSampler_GetLengthFromReader;
  sampler->base.Frequency = Binv1SoundSampler_GetFrequencyFromReader;
  sampler->base.Volume = Binv1SoundSampler_GetVolumeFromReader;

  return &sampler->base;
}

static inline bool
Binv1SoundSampler_WriteVolumeAndFrequencies(
    SoundSampler *sampler,
    const Writer *writer)
{
  unsigned char volume = SoundSampler_GetVolume(sampler);

  if (!Writer_WriteUInt32(writer, volume)) {
    return false;
  }

  for (enum Note note = 0; note < NOTE_COUNT; note++) {
    Tone tone = {
      .octave = 0,
      .note = note,
    };

    unsigned int frequency = SoundSampler_GetFrequency(sampler, &tone);

    if (!Writer_WriteUInt32(writer, frequency)) {
      return false;
    }
  }

  return true;
}

static inline bool
Binv1SoundSampler_WriteSamples(
    SoundSampler *sampler,
    const Writer *writer)
{
  unsigned int length = SoundSampler_GetLength(sampler);

  if (!Writer_WriteUInt32(writer, length)) {
    return false;
  }

  for (unsigned int index = 0; index < length; index++) {
    int sample = SoundSampler_GetSample(sampler, index);
    if (!Writer_WriteInt32(writer, sample)) {
      return false;
    }
  }

  return true;
}

bool
Binv1SoundSampler_To(
    SoundSampler *sampler,
    const Writer *writer)
{
  return true
    && Binv1SoundSampler_WriteVolumeAndFrequencies(sampler, writer)
    && Binv1SoundSampler_WriteSamples(sampler, writer);
}
