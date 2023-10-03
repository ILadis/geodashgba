#ifndef SOUND_H
#define SOUND_H

#include <types.h>
#include <math.h>

typedef enum Note {
  NOTE_C = 4,
  NOTE_CSHARP,
  NOTE_D = 6,
  NOTE_DSHARP,
  NOTE_E = 8,
  NOTE_F = 10,
  NOTE_FSHARP,
  NOTE_G = 12,
  NOTE_GSHARP,
  NOTE_A = 0,
  NOTE_ASHARP,
  NOTE_B = 2,
} Note;

static inline Note
Note_FromText(char *text) {
  char symbol = text[0];
  char modifier = text[1];

  enum Note note = (symbol - 'A') * 2 + (modifier == '^' ? 1 : 0);

  return note;
}

typedef struct Sample {
  void *self;
  int (*Get)(void *self, int index);
} Sample;

static inline int
Sample_Get(Sample *sample, int index) {
  return sample->Get(sample->self, index);
}

typedef struct NoteSample {
  Sample super;
  Note note;
  int rate, length;
} NoteSample;

Sample*
NoteSample_Create(
    NoteSample *sample,
    Note note,
    int rate);

Sample*
NoteSample_NextFrom(
    NoteSample *sample,
    char *notes,
    int rate);

typedef struct SoundChannel {
  void *self;
  void (*Fill)(void *self, int *buffer, int size);
} SoundChannel;

static inline void
SoundChannel_Fill(SoundChannel *channel, int *buffer, int size) {
  channel->Fill(channel->self, buffer, size);
}

typedef struct NoteSoundChannel {
  struct SoundChannel super;
  struct NoteSample sample;
  char *notes;
  int position;
  int volume; // TODO not implemented yet
} NoteSoundChannel;

SoundChannel*
NoteSoundChannel_Create(
    NoteSoundChannel *channel,
    char *notes,
    int rate);

#endif
