
#include <sound.h>
#include "../test.h"

static unsigned char module[3292];

test(NextTone_ShouldReturnNullAfterExpectedAmountOfNotes) {
  // arrange
  DataSource *source = Buffer_From(&(Buffer) {0}, module, length(module));
  Reader *reader = DataSource_AsReader(source);

  SoundTrack *track = ModuleSoundTrack_From(&(ModuleSoundTrack) {0}, reader, 0);

  unsigned int count = 0;

  // act
  while (SoundTrack_NextTone(track) != NULL) {
    count++;
  }

  // assert
  assert(count == 128);
}

test(NextTone_ShouldReturnTonesOfFirstChannelWithExpectedNoteAndOctave) {
  // arrange
  DataSource *source = Buffer_From(&(Buffer) {0}, module, length(module));
  Reader *reader = DataSource_AsReader(source);

  SoundTrack *track = ModuleSoundTrack_From(&(ModuleSoundTrack) {0}, reader, 0);

  const Tone tones[] = {
    { .note = NOTE_G,      .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_G,      .octave = 2 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_G,      .octave = 2 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_G,      .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_G,      .octave = 2 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_G,      .octave = 2 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_F,      .octave = 1 },
  };

  // act
  for (unsigned int i = 0; i < length(tones); i++) {
    const Tone *tone = SoundTrack_NextTone(track);

    // assert
    assert(tone->note == tones[i].note);
    assert(tone->octave == tones[i].octave);
  }
}

test(NextTone_ShouldReturnTonesOfSecondChannelWithExpectedNoteAndOctave) {
  // arrange
  DataSource *source = Buffer_From(&(Buffer) {0}, module, length(module));
  Reader *reader = DataSource_AsReader(source);

  SoundTrack *track = ModuleSoundTrack_From(&(ModuleSoundTrack) {0}, reader, 1);

  const Tone tones[] = {
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_D,      .octave = 2 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_G,      .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_PAUSE,  .octave = 1 },
    { .note = NOTE_G,      .octave = 1 },
    { .note = NOTE_G,      .octave = 1 },
    { .note = NOTE_A,      .octave = 1 },
    { .note = NOTE_ASHARP, .octave = 1 },
  };

  // act
  for (unsigned int i = 0; i < length(tones); i++) {
    const Tone *tone = SoundTrack_NextTone(track);

    // assert
    assert(tone->note == tones[i].note);
    assert(tone->octave == tones[i].octave);
  }
}

test(GetSample_ShouldReturnExpectedDataForFirstSample) {
  // arrange
  DataSource *source = Buffer_From(&(Buffer) {0}, module, length(module));
  Reader *reader = DataSource_AsReader(source);

  SoundSampler *sampler = ModuleSoundSampler_From(&(ModuleSoundSampler) {0}, reader, 0);

  char samples[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x9d, 0xa3, 0xa9, 0xaf, 0xb5, 0xbb, 0xc1, 0xc7,
    0xcd, 0xd2, 0xd8, 0xde, 0xe4, 0xea, 0xf0, 0xf6,
  };

  // act
  for (unsigned int i = 0; i < length(samples); i++) {
    int sample = SoundSampler_GetSample(sampler, i);

    // assert
    assert(sample == samples[i]);
  }

  unsigned char volume = SoundSampler_GetVolume(sampler);
  assert(volume == 64);
}

test(GetSample_ShouldReturnExpectedDataForSecondSample) {
  // arrange
  DataSource *source = Buffer_From(&(Buffer) {0}, module, length(module));
  Reader *reader = DataSource_AsReader(source);

  SoundSampler *sampler = ModuleSoundSampler_From(&(ModuleSoundSampler) {0}, reader, 1);

  char samples[] = {
    0x00, 0x00, 0x01, 0x40, 0x53, 0x42, 0x29, 0x39, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x34, 0xec, 0xd6, 0xb3, 0xc1, 0xc0,
    0xd0, 0xc8, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9,
    0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9,
    0xc9, 0xc9, 0xc9, 0xc9,
  };

  // act
  for (unsigned int i = 0; i < length(samples); i++) {
    int sample = SoundSampler_GetSample(sampler, i);

    // assert
    assert(sample == samples[i]);
  }

  unsigned char volume = SoundSampler_GetVolume(sampler);
  assert(volume == 64);
}

test(GetSample_ShouldReturnExpectedDataForThirdSample) {
  // arrange
  DataSource *source = Buffer_From(&(Buffer) {0}, module, length(module));
  Reader *reader = DataSource_AsReader(source);

  SoundSampler *sampler = ModuleSoundSampler_From(&(ModuleSoundSampler) {0}, reader, 2);

  char samples[] = {
    0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x1f, 0x27, 0x2c, 0x32, 0x37, 0x3d,
    0x42, 0x40, 0x40, 0x41, 0x41, 0x41, 0x44, 0x47, 0x48, 0x3e, 0x27, 0x0e,
    0xf8, 0xe1, 0xc6, 0xad, 0x9a, 0x9d, 0xc7, 0x19,
  };

  // act
  for (unsigned int i = 0; i < length(samples); i++) {
    int sample = SoundSampler_GetSample(sampler, i);

    // assert
    assert(sample == samples[i]);
  }

  unsigned char volume = SoundSampler_GetVolume(sampler);
  assert(volume == 48);
}

test(GetSample_ShouldReturnExpectedDataForFourthSample) {
  // arrange
  DataSource *source = Buffer_From(&(Buffer) {0}, module, length(module));
  Reader *reader = DataSource_AsReader(source);

  SoundSampler *sampler = ModuleSoundSampler_From(&(ModuleSoundSampler) {0}, reader, 3);

  char samples[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xda, 0xdc, 0xdf, 0xe1, 0xe3, 0xe6, 0xe8, 0xea,
    0xed, 0xef, 0xf1, 0xf3, 0xf6, 0xf8, 0xfa, 0xfd,
  };

  // act
  for (unsigned int i = 0; i < length(samples); i++) {
    int sample = SoundSampler_GetSample(sampler, i);

    // assert
    assert(sample == samples[i]);
  }

  unsigned char volume = SoundSampler_GetVolume(sampler);
  assert(volume == 64);
}

test(FillSample_ShouldReturnExpectedSamplesForGivenIncrement) {
  // arrange
  DataSource *source = Buffer_From(&(Buffer) {0}, module, length(module));
  Reader *reader = DataSource_AsReader(source);

  SoundSampler *sampler = ModuleSoundSampler_From(&(ModuleSoundSampler) {0}, reader, 1);

  int buffer[6] = {0};
  unsigned int position = 0;
  unsigned int increment = (1 << SOUND_CHANNEL_PRECISION) * 2;
  unsigned char volume = 1 << SOUND_VOLUME_PRECISION;

  // act
  int *next = SoundSampler_FillBuffer(sampler, buffer, &position, increment, volume, length(buffer));

  // assert
  assert(buffer[0] == 0x00);
  assert(buffer[1] == 0x01);
  assert(buffer[2] == 0x53);
  assert(buffer[3] == 0x29);
  assert(buffer[4] == 0x36);
  assert(buffer[5] == 0x36);
  assert(next == buffer + 6);
}

test(FillSample_ShouldReturnExpectedSamplesForGivenVolume) {
  // arrange
  DataSource *source = Buffer_From(&(Buffer) {0}, module, length(module));
  Reader *reader = DataSource_AsReader(source);

  SoundSampler *sampler = ModuleSoundSampler_From(&(ModuleSoundSampler) {0}, reader, 1);

  int buffer[5] = {0};
  unsigned int position = 0;
  unsigned int increment = (1 << SOUND_CHANNEL_PRECISION) * 2;
  unsigned char volume = (1 << SOUND_VOLUME_PRECISION) / 2;

  // act
  int *next = SoundSampler_FillBuffer(sampler, buffer, &position, increment, volume, length(buffer));

  // assert
  assert(buffer[0] == 0x00);
  assert(buffer[1] == 0x00);
  assert(buffer[2] == 0x29);
  assert(buffer[3] == 0x14);
  assert(buffer[4] == 0x1b);
  assert(next == buffer + 5);
}

suite(
  NextTone_ShouldReturnNullAfterExpectedAmountOfNotes,
  NextTone_ShouldReturnTonesOfFirstChannelWithExpectedNoteAndOctave,
  NextTone_ShouldReturnTonesOfSecondChannelWithExpectedNoteAndOctave,
  GetSample_ShouldReturnExpectedDataForFirstSample,
  GetSample_ShouldReturnExpectedDataForSecondSample,
  GetSample_ShouldReturnExpectedDataForThirdSample,
  GetSample_ShouldReturnExpectedDataForFourthSample,
  FillSample_ShouldReturnExpectedSamplesForGivenIncrement,
  FillSample_ShouldReturnExpectedSamplesForGivenVolume);

static unsigned char module[] = {
  0x4d, 0x61, 0x69, 0x6e, 0x20, 0x54, 0x68, 0x65, 0x6d, 0x65, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x61, 0x6d, 0x70,
  0x6c, 0x65, 0x20, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x40, 0x00, 0x00,
  0x00, 0x10, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x20, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x20, 0x01, 0x40, 0x00, 0x00, 0x00, 0x20, 0x53, 0x61, 0x6d, 0x70,
  0x6c, 0x65, 0x20, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x30, 0x00, 0x00,
  0x00, 0x10, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x20, 0x34, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x10, 0x01, 0x40, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x4d, 0x2e, 0x4b, 0x2e, 0x02, 0x3a, 0x2a, 0x08, 0x00, 0x00, 0x0e, 0x01,
  0x01, 0x7d, 0x1f, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x1d, 0x2a, 0x08, 0x01, 0x7d, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x02, 0x3a, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x04, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x3a, 0x10, 0x00, 0x01, 0x1d, 0x2c, 0x0c,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x04, 0x01, 0x1d, 0x2a, 0x08, 0x02, 0x3a, 0x40, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x02, 0x3a, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a,
  0x01, 0x1d, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x04, 0x01, 0x1d, 0x2a, 0x08,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x02, 0x3a, 0x2c, 0x0c,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x3a, 0x1a, 0x0a,
  0x00, 0x00, 0x0a, 0x04, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x02, 0x3a, 0x1a, 0x0a, 0x01, 0x1d, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08,
  0x02, 0x3a, 0x4a, 0x0a, 0x01, 0xfc, 0x1a, 0x0a, 0x00, 0x00, 0x0a, 0x04,
  0x01, 0x1d, 0x2a, 0x08, 0x02, 0x3a, 0x4a, 0x0a, 0x01, 0xe0, 0x1a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x01, 0xfc, 0x4a, 0x0a,
  0x01, 0xac, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x2a, 0x08,
  0x01, 0xe0, 0x4a, 0x0a, 0x01, 0x7d, 0x10, 0x00, 0x01, 0x1d, 0x2c, 0x0c,
  0x00, 0x00, 0x0a, 0x08, 0x01, 0xac, 0x4a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x04, 0x01, 0x40, 0x2a, 0x08, 0x01, 0x1d, 0x3a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x04,
  0x00, 0x00, 0x0a, 0x08, 0x01, 0x1d, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x1d, 0x4a, 0x0a, 0x00, 0x00, 0x0a, 0x08, 0x00, 0xfe, 0x3a, 0x0a,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x2a, 0x08,
  0x00, 0xf0, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1d, 0x4a, 0x0a,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0xd6, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xfe, 0x4a, 0x0a, 0x02, 0x80, 0x2a, 0x08, 0x00, 0xbe, 0x30, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x4a, 0x0a, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0x4a, 0x0a,
  0x01, 0x40, 0x2a, 0x08, 0x00, 0x00, 0x0a, 0x0a, 0x01, 0x7d, 0x10, 0x00,
  0x00, 0xbe, 0x40, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x08,
  0x01, 0x7d, 0x40, 0x00, 0x01, 0x7d, 0x10, 0x00, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x2a, 0x08, 0x01, 0x7d, 0x40, 0x00,
  0x01, 0x68, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x0a, 0x0a, 0x01, 0x40, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x02, 0xd0, 0x2a, 0x08, 0x01, 0x68, 0x4a, 0x0a, 0x01, 0x1d, 0x10, 0x00,
  0x01, 0x40, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08, 0x01, 0x40, 0x4a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x04, 0x01, 0x68, 0x2a, 0x08,
  0x01, 0x1d, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xd0, 0x2c, 0x0c,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x04, 0x00, 0x00, 0x0a, 0x08, 0x01, 0x1d, 0x3a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x68, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0xf0, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x04,
  0x01, 0x68, 0x2a, 0x08, 0x00, 0xd6, 0x3a, 0x0a, 0x00, 0x00, 0x0a, 0x0a,
  0x01, 0x1d, 0x4a, 0x0a, 0x00, 0x00, 0x0a, 0x08, 0x00, 0xb4, 0x3a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x4a, 0x0a, 0x02, 0xd0, 0x2a, 0x08,
  0x00, 0x8f, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0x4a, 0x0a,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xb4, 0x4a, 0x0a, 0x01, 0x68, 0x2a, 0x08, 0x00, 0x00, 0x0a, 0x0a,
  0x01, 0x1d, 0x10, 0x00, 0x00, 0x8f, 0x40, 0x00, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x08, 0x01, 0x1d, 0x40, 0x00, 0x01, 0x1d, 0x10, 0x00,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x01, 0x68, 0x2a, 0x08,
  0x01, 0x1d, 0x40, 0x00, 0x01, 0x40, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a, 0x01, 0x68, 0x1a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x2a, 0x08, 0x01, 0x40, 0x4a, 0x0a,
  0x01, 0x40, 0x10, 0x00, 0x01, 0x68, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08,
  0x01, 0x68, 0x4a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x04,
  0x00, 0xf0, 0x2a, 0x08, 0x01, 0x40, 0x40, 0x00, 0x00, 0x00, 0x0a, 0x0a,
  0x01, 0xe0, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x68, 0x1a, 0x0a, 0x00, 0x00, 0x0a, 0x04, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x0a, 0x0a, 0x01, 0x7d, 0x10, 0x00, 0x00, 0xf0, 0x2c, 0x0c,
  0x00, 0x00, 0x0a, 0x08, 0x01, 0x68, 0x4a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x04, 0x00, 0xf0, 0x2a, 0x08, 0x00, 0xf0, 0x3a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0xd6, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0xe0, 0x2a, 0x08, 0x00, 0xbe, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xf0, 0x4a, 0x0a, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0x4a, 0x0a, 0x00, 0xf0, 0x2a, 0x08,
  0x00, 0xf0, 0x30, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0xbe, 0x40, 0x00,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 0x08, 0x01, 0x40, 0x30, 0x00,
  0x01, 0x7d, 0x10, 0x00, 0x00, 0xf0, 0x40, 0x00, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0xf0, 0x2a, 0x08, 0x01, 0x7d, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x40, 0x40, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 0x0a, 0x02, 0x1a, 0x2a, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xac, 0x10, 0x00, 0x00, 0xf0, 0x2c, 0x0c,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x04, 0x01, 0x0d, 0x2a, 0x08, 0x01, 0xac, 0x40, 0x00,
  0x00, 0x00, 0x0a, 0x0a, 0x02, 0x1a, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x7d, 0x1a, 0x0a, 0x00, 0x00, 0x0a, 0x04,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a, 0x01, 0x68, 0x10, 0x00,
  0x01, 0x0d, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08, 0x01, 0x7d, 0x4a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x04, 0x01, 0x0d, 0x2a, 0x08,
  0x01, 0xac, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x08, 0x01, 0x7d, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x1a, 0x2a, 0x08, 0x01, 0x68, 0x30, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xac, 0x4a, 0x0a, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x01, 0x7d, 0x4a, 0x0a,
  0x01, 0x0d, 0x2a, 0x08, 0x01, 0xac, 0x3a, 0x0a, 0x00, 0x00, 0x0a, 0x0a,
  0x01, 0x68, 0x40, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x01, 0x7d, 0x3a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 0x08,
  0x01, 0x68, 0x30, 0x00, 0x01, 0x7d, 0x10, 0x00, 0x01, 0xac, 0x4a, 0x0a,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 0x0a,
  0x01, 0x7d, 0x4a, 0x0a, 0x01, 0x0d, 0x2a, 0x08, 0x01, 0x7d, 0x40, 0x00,
  0x01, 0xac, 0x10, 0x00, 0x01, 0x68, 0x40, 0x00, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 0x0a,
  0x02, 0x3a, 0x2a, 0x08, 0x01, 0xac, 0x40, 0x00, 0x01, 0xe0, 0x10, 0x00,
  0x01, 0x0d, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x04, 0x01, 0x1d, 0x2a, 0x08,
  0x01, 0xe0, 0x40, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x02, 0x3a, 0x2c, 0x0c,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0xac, 0x1a, 0x0a,
  0x00, 0x00, 0x0a, 0x04, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a,
  0x01, 0x7d, 0x10, 0x00, 0x01, 0x1d, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08,
  0x01, 0xac, 0x4a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x04,
  0x01, 0x1d, 0x2a, 0x08, 0x01, 0xe0, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x01, 0xac, 0x3a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x3a, 0x2a, 0x08,
  0x01, 0x7d, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x4a, 0x0a,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x01, 0xac, 0x4a, 0x0a, 0x01, 0x1d, 0x2a, 0x08, 0x01, 0xe0, 0x3a, 0x0a,
  0x00, 0x00, 0x0a, 0x0a, 0x01, 0x7d, 0x40, 0x00, 0x00, 0x00, 0x0a, 0x08,
  0x01, 0xac, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0x00, 0x0a, 0x08, 0x01, 0x7d, 0x30, 0x00, 0x01, 0xac, 0x10, 0x00,
  0x01, 0xe0, 0x4a, 0x0a, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0x00, 0x0a, 0x0a, 0x01, 0xac, 0x4a, 0x0a, 0x01, 0x1d, 0x2a, 0x08,
  0x01, 0xac, 0x40, 0x00, 0x01, 0xe0, 0x10, 0x00, 0x01, 0x7d, 0x40, 0x00,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0x00, 0x0a, 0x0a, 0x01, 0xfc, 0x2a, 0x08, 0x01, 0xe0, 0x40, 0x00,
  0x01, 0xfc, 0x10, 0x00, 0x01, 0x1d, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x04,
  0x00, 0xfe, 0x2a, 0x08, 0x01, 0xfc, 0x40, 0x00, 0x00, 0x00, 0x0a, 0x0a,
  0x01, 0xfc, 0x2c, 0x0c, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x01, 0xac, 0x1a, 0x0a, 0x00, 0x00, 0x0a, 0x04, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x0a, 0x0a, 0x01, 0x94, 0x10, 0x00, 0x00, 0xfe, 0x2c, 0x0c,
  0x00, 0x00, 0x0a, 0x08, 0x01, 0xac, 0x4a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x04, 0x00, 0xfe, 0x2a, 0x08, 0x01, 0xe0, 0x3a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x08,
  0x01, 0xac, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0xfc, 0x2a, 0x08, 0x01, 0x94, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0xe0, 0x4a, 0x0a, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xac, 0x4a, 0x0a, 0x00, 0xfe, 0x2a, 0x08,
  0x01, 0xfc, 0x3a, 0x0a, 0x00, 0x00, 0x0a, 0x0a, 0x01, 0x94, 0x40, 0x00,
  0x00, 0x00, 0x0a, 0x08, 0x01, 0xc5, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x8f, 0x2c, 0x28, 0x01, 0xac, 0x3a, 0x0a,
  0x01, 0x53, 0x10, 0x00, 0x01, 0xfc, 0x4a, 0x0a, 0x00, 0x00, 0x0a, 0x08,
  0x01, 0x94, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc5, 0x4a, 0x0a,
  0x00, 0xfe, 0x2a, 0x08, 0x01, 0x7d, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x53, 0x40, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x01, 0x53, 0x3a, 0x0a,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x2a, 0x08,
  0x01, 0x7d, 0x30, 0x00, 0x01, 0x7d, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0x2a, 0x08, 0x01, 0x1d, 0x3a, 0x0a,
  0x01, 0xac, 0x3a, 0x0a, 0x01, 0x7d, 0x40, 0x00, 0x00, 0xd6, 0x2a, 0x08,
  0x01, 0x1d, 0x3a, 0x0a, 0x01, 0xac, 0x3a, 0x0a, 0x00, 0x00, 0x0a, 0x0a,
  0x00, 0xd6, 0x2a, 0x08, 0x01, 0x1d, 0x3a, 0x0a, 0x01, 0xac, 0x3a, 0x0a,
  0x01, 0x1d, 0x4a, 0x0a, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x1d, 0x4a, 0x0a, 0x00, 0xd6, 0x2a, 0x08,
  0x01, 0x1d, 0x3a, 0x0a, 0x01, 0xac, 0x3a, 0x0a, 0x01, 0x1d, 0x4a, 0x0a,
  0x00, 0xd6, 0x2a, 0x08, 0x01, 0x1d, 0x3a, 0x0a, 0x01, 0xac, 0x3a, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0x2a, 0x08, 0x01, 0x1d, 0x30, 0x00,
  0x01, 0xac, 0x30, 0x00, 0x01, 0x1d, 0x4a, 0x0a, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 0x0a, 0x01, 0x1d, 0x4a, 0x0a,
  0x01, 0xfc, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x1d, 0x40, 0x00, 0x01, 0xfc, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x01, 0xfc, 0x2a, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x2c, 0x0c,
  0x00, 0x00, 0x0a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0xfc, 0x2c, 0x0c, 0x01, 0xfc, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x2c, 0x0c, 0x01, 0xfc, 0x2a, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x9d, 0xa3, 0xa9, 0xaf, 0xb5, 0xbb, 0xc1, 0xc7,
  0xcd, 0xd2, 0xd8, 0xde, 0xe4, 0xea, 0xf0, 0xf6, 0x00, 0x00, 0x01, 0x40,
  0x53, 0x42, 0x29, 0x39, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
  0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
  0x36, 0x36, 0x34, 0xec, 0xd6, 0xb3, 0xc1, 0xc0, 0xd0, 0xc8, 0xc9, 0xc9,
  0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9,
  0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9,
  0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x1f, 0x27, 0x2c, 0x32, 0x37, 0x3d,
  0x42, 0x40, 0x40, 0x41, 0x41, 0x41, 0x44, 0x47, 0x48, 0x3e, 0x27, 0x0e,
  0xf8, 0xe1, 0xc6, 0xad, 0x9a, 0x9d, 0xc7, 0x19, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xda, 0xdc, 0xdf, 0xe1, 0xe3, 0xe6, 0xe8, 0xea, 0xed, 0xef, 0xf1, 0xf3,
  0xf6, 0xf8, 0xfa, 0xfd
};
