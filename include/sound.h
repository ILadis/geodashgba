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
  unsigned int rate;   // sample rate as power of two (for example 13 = 8kHz)
} NoteSample;

typedef struct SoundChannel {
  void *self;
  void (*Pitch)(void *self, unsigned int rate);
  unsigned int (*Fill)(void *self, int *buffer, unsigned int frequency);
} SoundChannel;

static inline void
SoundChannel_Pitch(SoundChannel *channel, unsigned int frequency) {
  return channel->Pitch(channel->self, frequency);
}

static inline unsigned int
SoundChannel_Fill(SoundChannel *channel, int *buffer, unsigned int size) {
  return channel->Fill(channel->self, buffer, size);
}

typedef struct NoteSoundChannel {
  struct SoundChannel super;
  struct {
    const char *notes;
    unsigned int index;
  } track;
  union Samples {
    NoteSample note;
  } samples;
  Sample *sample;
  unsigned int tempo;     // base used for calculating length of samples
  unsigned int rate;      // sample rate as power of two (for example 13 = 8kHz)
  unsigned int position;  // position in current sample (20.12 fixed point integer)
  unsigned int increment; // position increment per loop (20.12 fixed point integer)
} NoteSoundChannel;

// 20.12 fixed point integer (for both increment and position)
#define SOUND_CHANNEL_PRECISION 12

SoundChannel*
NoteSoundChannel_Create(
    NoteSoundChannel *channel,
    const char *notes,
    unsigned int tempo,
    int rate);

typedef struct SoundPlayer {
  char *buffers[2];       // buffers for mixing
  char *active;           // currently active buffer
  unsigned int size;      // buffer size
  unsigned int frequency; // playback frequency
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

bool
SoundPlayer_MixChannels(SoundPlayer *player);

void
SoundPlayer_VSync(SoundPlayer *player);

#endif
