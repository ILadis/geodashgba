#ifndef SOUND_H
#define SOUND_H

#include <types.h>
#include <math.h>
#include <gba.h>

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
  int (*Get)(void *self, unsigned int index);
} Sample;

static inline int
Sample_Get(Sample *sample, unsigned int index) {
  return sample->Get(sample->self, index);
}

typedef struct NoteSample {
  Sample super;
  Note note;
  unsigned int octave;
  unsigned int length; // total values in this sample
  unsigned int rate;
} NoteSample;

typedef struct SoundChannel {
  void *self;
  void (*Pitch)(void *self, unsigned int frequency);
  int (*Fill)(void *self, int *buffer, unsigned int size);
} SoundChannel;

static inline void
SoundChannel_Pitch(SoundChannel *channel, unsigned int frequency) {
  return channel->Pitch(channel->self, frequency);
}

static inline int
SoundChannel_Fill(SoundChannel *channel, int *buffer, unsigned int size) {
  return channel->Fill(channel->self, buffer, size);
}

typedef struct NoteSoundChannel {
  struct SoundChannel super;
  struct {
    char *notes;
    unsigned int index;
  } track;
  union Samples {
    NoteSample note;
  } samples;
  Sample *sample;
  unsigned int rate;      // sample rate as power of two (for example 13 = 8kHz)
  unsigned int tempo;
  unsigned int position;  // position in current sample (20.12 fixed point integer)
  unsigned int increment; // increment (20.12 fixed point integer)
} NoteSoundChannel;

SoundChannel*
NoteSoundChannel_Create(
    NoteSoundChannel *channel,
    char *notes,
    int rate);

typedef struct SoundPlayer {
  char *buffers[2];
  char *active;
  unsigned int size;
  unsigned int frequency;
  struct SoundChannel *channels[4];
} SoundPlayer;

SoundPlayer*
SoundPlayer_GetInstance();

void
SoundPlayer_Enable(SoundPlayer *player);

bool
SoundPlayer_AddChannel(
    SoundPlayer *player,
    SoundChannel *channel);

void
SoundPlayer_MixChannels(SoundPlayer *player);

void
SoundPlayer_VSync(SoundPlayer *player);

#endif
