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
  int octave;
  int length;
  int rate;
} NoteSample;

typedef struct SoundChannel {
  void *self;
  int (*Fill)(void *self, int *buffer, int size);
} SoundChannel;

static inline int
SoundChannel_Fill(SoundChannel *channel, int *buffer, int size) {
  return channel->Fill(channel->self, buffer, size);
}

typedef struct NoteSoundChannel {
  struct SoundChannel super;
  struct {
    char *notes;
    int index;
  } track;
  union Samples {
    NoteSample note;
  } samples;
  Sample *sample;
  int rate; // sample rate
  int tempo;
  // TODO implement volume
  int position;
  int length;
} NoteSoundChannel;

SoundChannel*
NoteSoundChannel_Create(
    NoteSoundChannel *channel,
    char *notes,
    int rate);

#endif
