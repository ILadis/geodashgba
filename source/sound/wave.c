
#include <sound.h>

#define WaveSoundSampler_SectionValue(s1, s2, s3, s4) (0 \
    | (s1 <<  0) \
    | (s2 <<  8) \
    | (s3 << 16) \
    | (s4 << 24))

#define SECTION_POSITION 12
#define SAMPLE_RATE_OFFSET 4
#define SAMPLE_SIZE_OFFSET 14

static inline bool
WaveSoundSampler_IsSection(int value, const char *section) {
  return WaveSoundSampler_SectionValue(section[0], section[1], section[2], section[3]) == value;
}

static inline unsigned int
WaveSoundSampler_GotoSection(
    const Reader *reader,
    const char *section)
{
  unsigned int position = SECTION_POSITION;
  if (!Reader_SeekTo(reader, position)) {
    return false;
  }

  while (true) {
    int value = 0;
    unsigned int length = 0;

    if (!Reader_ReadInt32(reader, &value) || !Reader_ReadUInt32(reader, &length)) {
      return 0;
    }
    else if (WaveSoundSampler_IsSection(value, section)) {
      return length;
    }

    position += 8 + length;
    Reader_SeekTo(reader, position);
  }
}

static bool
WaveSoundSampler_SampleRate(
    const Reader *reader,
    unsigned int *rate)
{
  const char *section = "fmt ";
  if (!WaveSoundSampler_GotoSection(reader, section)) {
    return false;
  }

  unsigned int position = Reader_GetPosition(reader) + SAMPLE_RATE_OFFSET;
  if (!Reader_SeekTo(reader, position) || !Reader_ReadUInt32(reader, rate)) {
    return false;
  }

  return true;
}

static bool
WaveSoundSampler_SampleSize(
    const Reader *reader,
    unsigned int *size)
{
  const char *section = "fmt ";
  if (!WaveSoundSampler_GotoSection(reader, section)) {
    return false;
  }

  unsigned short bits = 0;
  unsigned int position = Reader_GetPosition(reader) + SAMPLE_SIZE_OFFSET;
  if (!Reader_SeekTo(reader, position) || !Reader_ReadUInt16(reader, &bits)) {
    return false;
  }

  switch (bits) {
  case 8:
    *size = 1;
    return true;
  case 16:
    *size = 2;
    return true;
  }

  // other sample sizes are not supported
  return false;
}

static bool
WaveSoundSampler_VerifySignature(const Reader *reader) {
  int riff = 0;
  if (!Reader_SeekTo(reader, 0) || !Reader_ReadInt32(reader, &riff) || !WaveSoundSampler_IsSection(riff, "RIFF")) {
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

  const char *section = "fmt ";
  if (!WaveSoundSampler_GotoSection(reader, section)) {
    return false;
  }

  unsigned short format = 0;
  unsigned short channels = 0;

  if (!Reader_ReadUInt16(reader, &format) || !Reader_ReadUInt16(reader, &channels)) {
    return false;
  }

  // only uncompressed data and single channel are supported
  if (format != 1 || channels != 1) {
    return false;
  }

  /* meta data that follows:
   * - sample rate in bytes/s (uint32)
   * - average bytes per second (uint32)
   * - block alignment (uint16)
   * - sample size in bits (uint16)
   */

  unsigned int value = 0;
  if (!WaveSoundSampler_SampleRate(reader, &value) || !WaveSoundSampler_SampleSize(reader, &value)) {
    return false;
  }

  return true;
}

static int
WaveSoundSampler_GetSample(
    void *self,
    unsigned int index)
{
  WaveSoundSampler *wave = self;
  const Reader *reader = wave->reader;

  unsigned int size = 0;
  if (!WaveSoundSampler_SampleSize(reader, &size)) {
    return 0;
  }

  const char *section = "data";
  unsigned int length = WaveSoundSampler_GotoSection(reader, section);

  unsigned int position = Reader_GetPosition(reader);
  unsigned int offset = (index * size) % length;

  Reader_SeekTo(reader, position + offset);
  int value = 0;

  switch (size) {
    // sample data is unsigned
    case 1: {
      unsigned char byte = 0;
      Reader_ReadUInt8(reader, &byte);
      value = byte - 0x80;
    } break;
    // sample data is signed
    case 2: {
      short byte = 0;
      Reader_ReadInt16(reader, &byte);
      // values range from +32768 to -32768 (15 bits, treat value as 1.15 fixed point integer)
      value = (byte * 127) >> 15;
    } break;
  }

  return value;
}

static unsigned int
WaveSoundSampler_GetLength(void *self) {
  WaveSoundSampler *wave = self;
  const Reader *reader = wave->reader;

  unsigned int size = 0;
  if (!WaveSoundSampler_SampleSize(reader, &size)) {
    return 0;
  }

  const char *section = "data";
  unsigned int length = WaveSoundSampler_GotoSection(reader, section);

  switch (size) {
    case 1: return length >> 0;
    case 2: return length >> 1;
  }

  return 0;
}

static unsigned int
WaveSoundSampler_GetFrequency(
    void *self,
    unused const Tone *tone)
{
  WaveSoundSampler *wave = self;
  const Reader *reader = wave->reader;

  unsigned int rate = 0;
  if (!WaveSoundSampler_SampleRate(reader, &rate)) {
    return 0;
  }

  return rate; // TODO scale by tone/note
}

static int*
WaveSoundSampler_FillBuffer(
    void *self, int *buffer,
    unsigned int *position,
    unsigned int increment,
    unsigned char volume,
    unsigned int size)
{
  WaveSoundSampler *wave = self;
  return SoundSampler_SlowFillBuffer(&wave->base, buffer, position, increment, volume, size);
}

static unsigned char
WaveSoundSampler_GetVolume(unused void *self) {
  return 1 << SOUND_VOLUME_PRECISION;
}

SoundSampler*
WaveSoundSampler_From(
    WaveSoundSampler *sampler,
    const Reader *reader)
{
  sampler->reader = reader;

  if (!WaveSoundSampler_VerifySignature(reader)) {
    return NULL;
  }

  sampler->base.self = sampler;
  sampler->base.Get  = WaveSoundSampler_GetSample;
  sampler->base.Fill = WaveSoundSampler_FillBuffer;
  sampler->base.Length = WaveSoundSampler_GetLength;
  sampler->base.Frequency = WaveSoundSampler_GetFrequency;
  sampler->base.Volume = WaveSoundSampler_GetVolume;

  return &sampler->base;
}
