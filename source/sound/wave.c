
#include <sound.h>

#define WaveSoundSampler_SectionValue(s1, s2, s3, s4) (0 \
    | (s1 <<  0) \
    | (s2 <<  8) \
    | (s3 << 16) \
    | (s4 << 24))

static inline bool
WaveSoundSampler_IsSection(int value, const char *section) {
  return WaveSoundSampler_SectionValue(section[0], section[1], section[2], section[3]) == value;
}

static bool
WaveSoundSampler_ReadFormatSection(
    WaveSoundSampler *sampler,
    unused unsigned int position,
    unused unsigned int length)
{
  const Reader *reader = sampler->reader;

  unsigned short format = 0;
  unsigned short channels = 0;

  if (!Reader_ReadUInt16(reader, &format) || !Reader_ReadUInt16(reader, &channels)) {
    return false;
  }

  // only uncompressed data and single channel are supported
  if (format != 1 || channels != 1) {
    return false;
  }

  unsigned int rate = 0;
  if (!Reader_ReadUInt32(reader, &rate)) {
    return false;
  }

  unsigned int skip1 = 0;
  Reader_ReadUInt32(reader, &skip1); // average bytes per second (byte rate)
  unsigned short skip2 = 0;
  Reader_ReadUInt16(reader, &skip2); // block alignment

  unsigned short bits = 0;
  if (!Reader_ReadUInt16(reader, &bits)) {
    return false;
  }

  sampler->sample.size = bits / 8;
  sampler->sample.rate = rate;

  return true;
}

static bool
WaveSoundSampler_ReadDataSection(
    WaveSoundSampler *sampler,
    unused unsigned int position,
    unused unsigned int length)
{
  // just save position of data section as offset
  sampler->offset = position;
  return true;
}

static bool
WaveSoundSampler_ReadMetaData(WaveSoundSampler *sampler) {
  const Reader *reader = sampler->reader;

  int riff = 0;
  if (!Reader_ReadInt32(reader, &riff) || !WaveSoundSampler_IsSection(riff, "RIFF")) {
    return false;
  }

  unsigned int size = 0;
  if (!Reader_ReadUInt32(reader, &size)) {
    return false;
  }

  int wave = 0;
  if (!Reader_ReadInt32(reader, &wave) || !WaveSoundSampler_IsSection(wave, "WAVE")) {
    return false;
  }

  unsigned int position = 12;

  while (true) {
    int section = 0;
    unsigned int length = 0;

    if (!Reader_ReadInt32(reader, &section) || !Reader_ReadUInt32(reader, &length)) {
      break;
    }

    position += 8;

    switch (section) {
    case WaveSoundSampler_SectionValue('f', 'm', 't', ' '):
      WaveSoundSampler_ReadFormatSection(sampler, position, length);
      break;
    case WaveSoundSampler_SectionValue('d', 'a', 't', 'a'):
      WaveSoundSampler_ReadDataSection(sampler, position, length);
      break;
    }

    position += length;
    Reader_SeekTo(reader, position);
  }

  return true;
}

static void
WaveSoundSampler_Prepare(
    void *self,
    SoundChannel *channel,
    const Tone *tone)
{
  WaveSoundSampler *wave = self;

  unsigned int frequency = Tone_GetFrequency(tone);
  unsigned int base = Tone_GetBaseFrequency() >> 8;

  unsigned int ratio = Math_div(frequency, base);
  unsigned int pitch = (ratio * wave->sample.rate) >> 8;

  SoundChannel_Pitch(channel, pitch);
}

static int
WaveSoundSampler_GetSample(
    void *self,
    unsigned int index)
{
  WaveSoundSampler *wave = self;

  unsigned int size = wave->sample.size;
  unsigned int position = index * size;

  const Reader *reader = wave->reader;
  Reader_SeekTo(reader, wave->offset + position);

  int value = 0;

  switch (size) {
    case 1: {
      char byte = 0;
      Reader_ReadInt8(reader, &byte);
      value = byte;
    } break;
    case 2: {
      short byte = 0;
      Reader_ReadInt16(reader, &byte);
      // values range from +32768 to -32768 (15 bits, treat value as 1.14 fixed point integer)
      value = (byte * 127) >> 14;
    } break;
  }

  return value;
}

SoundSampler*
WaveSoundSampler_FromReader(
    WaveSoundSampler *wave,
    Reader *reader)
{
  wave->reader = reader;
  wave->base.self = wave;
  wave->base.Prepare = WaveSoundSampler_Prepare;
  wave->base.Get = WaveSoundSampler_GetSample;

  if (!WaveSoundSampler_ReadMetaData(wave)) {
    return NULL;
  }

  return &wave->base;
}
