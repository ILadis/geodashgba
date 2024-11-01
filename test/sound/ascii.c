
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

suite(
  NextTone_ShouldReturnToneWithExpectedNoteAndSampleAndEffect,
  NextTone_ShouldReturnNullWhenNoMoreTonesAreLeft,
  To_ShouldProduceExpectedSoundTrack);
