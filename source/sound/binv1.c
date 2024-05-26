
#include <sound.h>

static bool
Binv1SoundTrack_AddTone(
    void *self,
    const Tone *tone)
{
  Binv1SoundTrack *track = self;

  Writer *writer = DataSource_AsWriter(track->source);
  Writer_WriteUInt8(writer, (unsigned char) tone->note);
  Writer_WriteUInt8(writer, (unsigned char) tone->effect.type);
  Writer_WriteUInt8(writer, (unsigned char) tone->effect.param);
  Writer_WriteUInt8(writer, (unsigned char) tone->octave);
  Writer_WriteUInt8(writer, (unsigned char) tone->sample);

  return true;
}

static const Tone*
Binv1SoundTrack_NextTone(void *self) {
  Binv1SoundTrack *track = self;
  Tone *tone = &track->tone;

  Reader *reader = DataSource_AsReader(track->source);
  Reader_ReadUInt8(reader, (unsigned char *) &tone->note);
  Reader_ReadUInt8(reader, (unsigned char *) &tone->effect.type);
  Reader_ReadUInt8(reader, (unsigned char *) &tone->effect.param);
  Reader_ReadUInt8(reader, (unsigned char *) &tone->octave);
  Reader_ReadUInt8(reader, (unsigned char *) &tone->sample);

  return tone;
}

static bool
Binv1SoundTrack_SeekTo(
    void *self,
    unsigned int position)
{
  Binv1SoundTrack *track = self;

  Reader *reader = DataSource_AsReader(track->source);
  return Reader_SeekTo(reader, position * 5); // 5 bytes per tone
}

SoundTrack*
Binv1SoundTrack_From(
    Binv1SoundTrack *track,
    DataSource *source)
{
  track->source = source;

  track->base.self = track;
  track->base.Add  = Binv1SoundTrack_AddTone;
  track->base.Next = Binv1SoundTrack_NextTone;
  track->base.SeekTo = Binv1SoundTrack_SeekTo;

  return &track->base;
}

static int
Binv1SoundSampler_GetSample(void *self, unsigned int index) {
  Binv1SoundSampler *sampler = self;

  Reader *reader = DataSource_AsReader(sampler->source);

  unsigned short length = 0;
  Reader_SeekTo(reader, 0);
  Reader_ReadUInt16(reader, &length);

  unsigned int position = index & (length - 1); // length is mask for index (modulo of some power of 2)

  int sample = 0;
  Reader_SeekTo(reader, 3 + NOTE_COUNT * 4 + position); // 4 bytes per frequency
  Reader_ReadInt32(reader, &sample);

  return sample;
}

static unsigned int
Binv1SoundSampler_GetFrequency(void *self, const Tone *tone) {
  Binv1SoundSampler *sampler = self;

  Reader *reader = DataSource_AsReader(sampler->source);
  Reader_SeekTo(reader, 3 + tone->note * 4); // 4 bytes per frequency

  unsigned int frequency = 0;
  Reader_ReadUInt32(reader, &frequency);

  return frequency << tone->octave;
}

static inline unsigned char
Binv1SoundSampler_GetVolume(void *self) {
  Binv1SoundSampler *sampler = self;

  Reader *reader = DataSource_AsReader(sampler->source);
  Reader_SeekTo(reader, 2);

  unsigned char volume = 0;
  Reader_ReadUInt8(reader, &volume);

  return volume;
}

SoundSampler*
Binv1SoundSampler_From(
    Binv1SoundSampler *sampler,
    DataSource *source)
{
  sampler->source = source;

  sampler->base.self = sampler;

  sampler->base.Get  = Binv1SoundSampler_GetSample;
  sampler->base.Frequency = Binv1SoundSampler_GetFrequency;
  sampler->base.Volume = Binv1SoundSampler_GetVolume;

  return &sampler->base;
}

static inline void
Binv1SoundSampler_ConvertVolumeAndFrequencies(
    Binv1SoundSampler *sampler,
    const SoundSampler *other)
{
  Writer *writer = DataSource_AsWriter(sampler->source);
  Writer_SeekTo(writer, 2);

  unsigned char volume = SoundSampler_GetVolume(other);
  Writer_WriteUInt8(writer, volume);

  for (enum Note note = 0; note < NOTE_COUNT; note++) {
    Tone tone = {
      .octave = 0,
      .note = note,
    };

    unsigned int frequency = SoundSampler_GetFrequency(other, &tone);
    Writer_WriteUInt32(writer, frequency);
  }
}

static inline void
Binv1SoundSampler_ConvertSamples(
    Binv1SoundSampler *sampler,
    const SoundSampler *other)
{
  Writer *writer = DataSource_AsWriter(sampler->source);

  int samples[1024] = {0};
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

  Writer_SeekTo(writer, 0);
  Writer_WriteUInt16(writer, shortest);

  Writer_SeekTo(writer, 3 + NOTE_COUNT * 4); // 4 bytes per frequency
  for (unsigned int index = 0; index < shortest; index++) {
    Writer_WriteInt32(writer, samples[index]);
  }
}

SoundSampler*
Binv1SoundSampler_ConvertFrom(
    Binv1SoundSampler *sampler,
    DataSource *source,
    const SoundSampler *other)
{
  SoundSampler *base = Binv1SoundSampler_From(sampler, source);

  Binv1SoundSampler_ConvertVolumeAndFrequencies(sampler, other);
  Binv1SoundSampler_ConvertSamples(sampler, other);

  return base;
}

