
#include <sound.h>

static const enum Note notes[] = {
  [ 4] = NOTE_C,
         NOTE_CSHARP,
  [ 6] = NOTE_D,
         NOTE_DSHARP,
  [ 8] = NOTE_E,
  [10] = NOTE_F,
         NOTE_FSHARP,
  [12] = NOTE_G,
         NOTE_GSHARP,
  [ 0] = NOTE_A,
         NOTE_ASHARP,
  [ 2] = NOTE_B,
};

static inline bool
Note_FromSymbol(
    Note *note,
    char symbol)
{
  // pause symbol
  if (symbol == 'Z') {
    *note = NOTE_PAUSE;
    return true;
  }

  unsigned int index = (symbol - 'A') * 2;
  if (index > length(notes)) {
    return false;
  }

  *note = notes[index];
  return true;
}

static const Tone*
AsciiSoundTrack_NextTone(void *self) {
  AsciiSoundTrack *track = self;

  unsigned int index = track->index;
  const char *notes = track->notes;

  enum Note note;
  char symbol = notes[index];
  if (!Note_FromSymbol(&note, symbol)) {
    return false;
  }

  unsigned int octave = 4;
  unsigned int dotted = 0;
  unsigned int ticks = (1 << NOTE_TICKS_PRECISION);
  bool shorten = false;

  while (true) {
    char modifier = notes[++index];
    if (modifier == ' ' || modifier == '|') {
      continue;
    }

    switch (modifier) {
      case '^': note += 1; continue;
      case '_': note -= 1; continue;
      case '`': octave += 1; continue;
      case ',': octave -= 1; continue;
      case '/': shorten = true; continue;
      case '2': ticks = shorten ? ticks >> 1 : ticks << 1; continue;
      case '4': ticks = shorten ? ticks >> 2 : ticks << 2; continue;
      case '8': ticks = shorten ? ticks >> 3 : ticks << 3; continue;
      case '.': ticks += ticks >> ++dotted; continue;
    }

    break;
  }

  Tone *tone = &track->tone;
  tone->note = note;
  tone->octave = octave;
  tone->ticks = ticks;
  track->index = index;

  return tone;
}

SoundTrack*
AsciiSoundTrack_FromNotes(
    AsciiSoundTrack *track,
    const char *notes)
{
  track->notes = notes;
  track->index = 0;

  track->base.self = track;
  track->base.Next = AsciiSoundTrack_NextTone;

  return &track->base;
}
