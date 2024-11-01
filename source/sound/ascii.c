
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
AsciiSoundTrack_NextNote(
    const AsciiSoundTrack *track,
    Note *note)
{
  const Reader *reader = track->reader;

  char symbol[2] = {0};
  if (!Reader_Read(reader, &symbol, length(symbol))) {
    return false;
  }

  // pause symbol
  if (symbol[0] == '-') {
    *note = NOTE_PAUSE;
    return true;
  }

  unsigned int index = (symbol[0] - 'A') * 2;
  unsigned int modifier = (symbol[1] == '#') ? 1 : 0;

  if (index > length(notes)) {
    return false;
  }

  *note = notes[index] + modifier;
  return true;
}

static inline bool
AsciiSoundTrack_NextValue(
    const AsciiSoundTrack *track,
    unsigned int digits,
    unsigned int *value)
{
  const Reader *reader = track->reader;
  unsigned int result = 0;

  while (digits > 0) {
    char digit;
    if (!Reader_Read(reader, &digit, 1)) {
      return false;
    }

    unsigned char hex = 0;

    // if "-" is encountered do not return result (leave value unchanged)
    if (digit == '-') value = &result;
    else if (digit >= '0' && digit <= '9') hex = digit - '0';
    else if (digit >= 'a' && digit <= 'f') hex = digit - 'a' + 10;
    else if (digit >= 'A' && digit <= 'F') hex = digit - 'A' + 10;
    else return false;

    result += hex * digits;
    digits >>= 4;
  }

  *value = result;
  return true;
}

static inline bool
AsciiSoundTrack_SkipNext(
    const AsciiSoundTrack *track,
    char next)
{
  const Reader *reader = track->reader;

  char value = '\0';
  if (!Reader_Read(reader, &value, 1)) {
    return false;
  }

  return value == next;
}

static const Tone*
AsciiSoundTrack_NextTone(void *self) {
  AsciiSoundTrack *track = self;

  enum Note note;
  if (!AsciiSoundTrack_NextNote(track, &note)) {
    return false;
  }

  unsigned int octave = 0;
  if (!AsciiSoundTrack_NextValue(track, 0x1, &octave)) {
    return false;
  }

  if (!AsciiSoundTrack_SkipNext(track, ' ')) {
    return false;
  }

  unsigned int sample = -1;
  if (!AsciiSoundTrack_NextValue(track, 0x10, &sample)) {
    return false;
  }

  if (!AsciiSoundTrack_SkipNext(track, ' ')) {
    return false;
  }

  unsigned int effect = 0;
  if (!AsciiSoundTrack_NextValue(track, 0x100, &effect)) {
    return false;
  }

  // skip separator character
  Reader_ReadOne(track->reader);

  Tone *tone = &track->tone;
  tone->note = note;
  tone->octave = octave;
  tone->sample = sample;
  tone->effect.type  = (effect & 0xF00) >> 8;
  tone->effect.param = (effect & 0x0FF) >> 0;

  return tone;
}

static bool
AsciiSoundTrack_SeekTo(
    void *self,
    unsigned int position)
{
  AsciiSoundTrack *track = self;

  const Reader *reader = track->reader;
  return Reader_SeekTo(reader, position * 11);
}

bool
AsciiSoundTrack_To(
    SoundTrack *track,
    const Writer *writer,
    char separator)
{
  static char hex[] = "0123456789ABCDEF";
  static const char notes[][2] = {
    [NOTE_C]      = "C-",
    [NOTE_CSHARP] = "C#",
    [NOTE_D]      = "D-",
    [NOTE_DSHARP] = "D#",
    [NOTE_E]      = "E-",
    [NOTE_F]      = "F-",
    [NOTE_FSHARP] = "F#",
    [NOTE_G]      = "G-",
    [NOTE_GSHARP] = "G#",
    [NOTE_A]      = "A-",
    [NOTE_ASHARP] = "A#",
    [NOTE_B]      = "B-",
  };

  #define hexify(value, index) hex[(value >> (index*4)) & 0xF];

  SoundTrack_SeekTo(track, 0);

  do {
    const Tone *tone = AsciiSoundTrack_NextTone(track);
    if (tone == NULL) break;

    char buffer[] = {
      // note/octave
      '-', '-', '-', ' ',
      // sample
      '-', '-', ' ',
      // effect type/param
      '-', '-', '-',
      separator,
    };

    Note note = tone->note;
    if (note < length(notes)) {
      buffer[0] = notes[note][0];
      buffer[1] = notes[note][1];
      buffer[2] = hexify(tone->octave, 0);
    }

    if (tone->sample < 32) {
      buffer[4] = hexify(tone->sample, 1);
      buffer[5] = hexify(tone->sample, 0);
    }

    SoundEffect effect = tone->effect;
    if (effect.type != 0 && effect.param != 0) {
      buffer[7] = hexify(effect.type, 0);
      buffer[8] = hexify(effect.param, 1);
      buffer[9] = hexify(effect.param, 0);
    }

    Writer_Write(writer, buffer, length(buffer));
  } while(true);

  SoundTrack_SeekTo(track, 0);
  return true;
}

SoundTrack*
AsciiSoundTrack_From(
    AsciiSoundTrack *track,
    const Reader *reader)
{
  track->reader = reader;

  track->base.self = track;
  track->base.Next = AsciiSoundTrack_NextTone;
  track->base.SeekTo = AsciiSoundTrack_SeekTo;

  return &track->base;
}
