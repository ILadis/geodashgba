
#include <sound.h>
#include "../test.h"

test(NextTone_ShouldReturnToneWithExpectedNoteAndSampleAndEffect) {
  // arrange
  char *notes = "D#7 05 C20";

  DataSource *source = Buffer_FromString(&(Buffer){0}, notes);
  const Reader *reader = DataSource_AsReader(source);

  SoundTrack *track = AsciiSoundTrack_From(&(AsciiSoundTrack){0}, reader);

  // act
  const Tone *tone = SoundTrack_NextTone(track);

  // assert
  assert(tone->note == NOTE_DSHARP);
  assert(tone->octave == 7);
  assert(tone->sample == 5);
  assert(tone->effect.type == SOUND_EFFECT_SET_VOLUME);
  assert(tone->effect.param == 0x20);
}

test(NextTone_ShouldReturnNullWhenNoMoreTonesAreLeft) {
  // arrange
  char *notes = ""
      "D#7 05 C20 "
      "C-6 -- --- "
      "--- -- --- "
      "F#5 01 --- "
      "G-1 00 D12 ";

  DataSource *source = Buffer_FromString(&(Buffer){0}, notes);
  const Reader *reader = DataSource_AsReader(source);

  SoundTrack *track = AsciiSoundTrack_From(&(AsciiSoundTrack){0}, reader);

  const Tone tones[] = {
    { .note = NOTE_DSHARP, .octave = 7, .sample =  5, .effect = { SOUND_EFFECT_SET_VOLUME, 0x20 } },
    { .note = NOTE_C,      .octave = 6, .sample = -1, .effect = { 0, 0 } },
    { .note = NOTE_PAUSE,  .octave = 0, .sample = -1, .effect = { 0, 0 } },
    { .note = NOTE_FSHARP, .octave = 5, .sample =  1, .effect = { 0, 0 } },
    { .note = NOTE_G,      .octave = 1, .sample =  0, .effect = { SOUND_EFFECT_BREAKTO_ROW, 0x12 } },
  };

  // act
  for (unsigned int i = 0; i < length(tones); i++) {
    const Tone *tone = SoundTrack_NextTone(track);

    // assert
    assert(tone->note == tones[i].note);
    assert(tone->octave == tones[i].octave);
    assert(tone->sample == tones[i].sample);
    assert(tone->effect.type == tones[i].effect.type);
    assert(tone->effect.param == tones[i].effect.param);
  }

  // act
  const Tone *tone = SoundTrack_NextTone(track);

  // assert
  assert(tone == NULL);
}

test(To_ShouldProduceExpectedSoundTrack) {
  // arrange
  char buffer[64] = {0};
  char *notes = ""
      "D#7 05 C20|"
      "C-6 -- ---|"
      "--- -- ---|"
      "F#5 01 ---|"
      "G-1 -- D12|";

  DataSource *source = Buffer_FromString(&(Buffer){0}, notes);
  const Reader *reader = DataSource_AsReader(source);

  SoundTrack *track = AsciiSoundTrack_From(&(AsciiSoundTrack){0}, reader);

  DataSource *target = Buffer_From(&(Buffer){0}, buffer, length(buffer));
  const Writer *writer = DataSource_AsWriter(target);

  // act
  AsciiSoundTrack_To(track, writer, '|');

  // assert
  for (unsigned int i = 0; notes[i] != '\0'; i++) {
    assert(notes[i] == buffer[i]);
  }
}

test(From_ShouldReturnExpectedVolumeAndLength) {
  // arrange
  char *values = "30083908B7093C09C70A5F0AF90BA10C540D110DD80EA80F89200000000006111F272C32373D4240404141414447483E270EF8E1C6AD9A9DC719";

  DataSource *source = Buffer_FromString(&(Buffer){0}, values);
  const Reader *reader = DataSource_AsReader(source);

  // act
  SoundSampler *sampler = AsciiSoundSampler_From(&(AsciiSoundSampler){0}, reader);

  unsigned char volume = SoundSampler_GetVolume(sampler);
  unsigned int length = SoundSampler_GetLength(sampler);

  // assert
  assert(volume == 48);
  assert(length == 32);
}

test(From_ShouldReturnExpectedFrequencies) {
  // arrange
  char *values = "30083908B7093C09C70A5F0AF90BA10C540D110DD80EA80F89200000000006111F272C32373D4240404141414447483E270EF8E1C6AD9A9DC719";
  const unsigned int frequencies[] = {2105, 2231, 2364, 2503, 2655, 2809, 2977, 3156, 3345, 3544, 3752, 3977};

  DataSource *source = Buffer_FromString(&(Buffer){0}, values);
  const Reader *reader = DataSource_AsReader(source);

  // act
  SoundSampler *sampler = AsciiSoundSampler_From(&(AsciiSoundSampler){0}, reader);

  for (enum Note note = 0; note < NOTE_COUNT; note++) {
    Tone tone = {
      .note = note,
      .octave = 0,
    };

    unsigned int frequency = SoundSampler_GetFrequency(sampler, &tone);

    // assert
    assert(frequency == frequencies[note]);
  }
}

test(From_ShouldReturnExpectedSamples) {
  // arrange
  char *values = "30083908B7093C09C70A5F0AF90BA10C540D110DD80EA80F89200000000006111F272C32373D4240404141414447483E270EF8E1C6AD9A9DC719";
  const int samples[] = {0, 0, 0, 0, 6, 17, 31, 39, 44, 50, 55, 61, 66, 64, 64, 65, 65, 65, 68, 71, 72, 62, 39, 14, -8, -31, -58, -83, -102, -99, -57, 25};

  DataSource *source = Buffer_FromString(&(Buffer){0}, values);
  const Reader *reader = DataSource_AsReader(source);

  // act
  SoundSampler *sampler = AsciiSoundSampler_From(&(AsciiSoundSampler){0}, reader);

  for (unsigned int i = 0; i < 32; i++) {
    int sample = SoundSampler_GetSample(sampler, i);

    // assert
    assert(sample == samples[i]);
  }
}

test(FillBuffer_ShouldReturnExpectedSamplesForGivenIncrement) {
  // arrange
  char *values = "40083908B7093C09C70A5F0AF90BA10C540D110DD80EA80F8910625C58514B453F39332E28221C16100A";

  DataSource *source = Buffer_FromString(&(Buffer){0}, values);
  const Reader *reader = DataSource_AsReader(source);

  SoundSampler *sampler = AsciiSoundSampler_From(&(AsciiSoundSampler){0}, reader);

  int buffer[6] = {0};
  unsigned int position = 0;
  unsigned int increment = (1 << SOUND_CHANNEL_PRECISION) / 2;
  unsigned char volume = 1 << SOUND_VOLUME_PRECISION;

  // act
  int *next = SoundSampler_FillBuffer(sampler, buffer, &position, increment, volume, length(buffer));

  // assert
  assert(buffer[0] == 98);
  assert(buffer[1] == 98);
  assert(buffer[2] == 92);
  assert(buffer[3] == 92);
  assert(buffer[4] == 88);
  assert(buffer[5] == 88);
  assert(next == buffer + 6);
}

test(FillBuffer_ShouldReturnExpectedSamplesForGivenVolume) {
  // arrange
  char *values = "40083908B7093C09C70A5F0AF90BA10C540D110DD80EA80F8910625C58514B453F39332E28221C16100A";

  DataSource *source = Buffer_FromString(&(Buffer){0}, values);
  const Reader *reader = DataSource_AsReader(source);

  SoundSampler *sampler = AsciiSoundSampler_From(&(AsciiSoundSampler){0}, reader);

  int buffer[3] = {0};
  unsigned int position = 0;
  unsigned int increment = 1 << SOUND_CHANNEL_PRECISION;
  unsigned char volume = (1 << SOUND_VOLUME_PRECISION) / 2;

  // act
  int *next = SoundSampler_FillBuffer(sampler, buffer, &position, increment, volume, length(buffer));

  // assert
  assert(buffer[0] == 49);
  assert(buffer[1] == 46);
  assert(buffer[2] == 44);
  assert(next == buffer + 3);
}

test(FillBuffer_ShouldReturnExpectedSamplesAndWrapAround) {
  // arrange
  char *values = "40083908B7093C09C70A5F0AF90BA10C540D110DD80EA80F8910625C58514B453F39332E28221C16100A";
  unsigned int samples[] = {98, 92, 88, 81, 75, 69, 63, 57, 51, 46, 40, 34, 28, 22, 16, 10};

  DataSource *source = Buffer_FromString(&(Buffer){0}, values);
  const Reader *reader = DataSource_AsReader(source);

  SoundSampler *sampler = AsciiSoundSampler_From(&(AsciiSoundSampler){0}, reader);

  int buffer[32] = {0};
  unsigned int position = 0;
  unsigned int increment = 1 << SOUND_CHANNEL_PRECISION;
  unsigned char volume = 1 << SOUND_VOLUME_PRECISION;

  // act
  int *next = SoundSampler_FillBuffer(sampler, buffer, &position, increment, volume, length(buffer));

  // assert
  for (unsigned int i = 0; i < 16; i++) {
    assert(buffer[i] == buffer[16 + i]);
    assert(buffer[i] == samples[i]);
  }
  assert(next == buffer + 32);
}

suite(
  NextTone_ShouldReturnToneWithExpectedNoteAndSampleAndEffect,
  NextTone_ShouldReturnNullWhenNoMoreTonesAreLeft,
  To_ShouldProduceExpectedSoundTrack,
  From_ShouldReturnExpectedVolumeAndLength,
  From_ShouldReturnExpectedFrequencies,
  From_ShouldReturnExpectedSamples,
  FillBuffer_ShouldReturnExpectedSamplesForGivenIncrement,
  FillBuffer_ShouldReturnExpectedSamplesForGivenVolume,
  FillBuffer_ShouldReturnExpectedSamplesAndWrapAround);
