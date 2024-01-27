
#include <sound.h>
#include "test.h"

test(NextTone_ShouldReturnToneWithExpectedNoteAndLength) {
  // arrange
  const char notes[] = "D`^2.";

  SoundTrack track;
  SoundTrack_AssignNotes(&track, notes, 2800);

  // act
  const Tone *tone = SoundTrack_NextTone(&track);

  // assert
  assert(tone->note == NOTE_DSHARP);
  assert(tone->octave == 5);
  assert(tone->length == 8400);
}

suite(
  NextTone_ShouldReturnToneWithExpectedNoteAndLength);
