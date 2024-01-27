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

typedef struct Tone {
  Note note;
  unsigned int octave;
  unsigned int length;
} Tone;

// TODO convert to AsciiSoundTrack to support multiple track formats
typedef struct SoundTrack {
  const char *notes;
  unsigned int index; // index of next char to consume in notes pointer
  unsigned int tempo; // base used for calculating length of tones (total values that can be sampled)
  struct Tone tone;   // current parsed note from sound track
} SoundTrack;

void
SoundTrack_AssignNotes(
    SoundTrack *track,
    const char *notes,
    unsigned int tempo);

const Tone*
SoundTrack_NextTone(SoundTrack *track);

typedef struct SoundSampler {
  void *self;
  int (*Get)(void *self, const Tone *tone, unsigned int index, unsigned int rate);
} SoundSampler;

static inline int
SoundSampler_Get(const SoundSampler *sample, const Tone *tone, unsigned int index, unsigned int rate) {
  return sample->Get(sample->self, tone, index, rate);
}

const SoundSampler*
SineSoundSampler_GetInstance();

typedef struct SoundChannel {
  SoundTrack *track;
  const Tone *tone;

  // TODO add different samplers: SineSampler, WaveSampler
  const SoundSampler *sampler;

  unsigned int rate;      // sample rate as power of two (for example 13 = 8kHz)
  unsigned int position;  // position in current sample (20.12 fixed point integer)
  unsigned int increment; // position increment per loop (20.12 fixed point integer)
} SoundChannel;

// 20.12 fixed point integer (for both increment and position)
#define SOUND_CHANNEL_PRECISION 12

void
SoundChannel_SetTrackAndSampler(
    SoundChannel *channel,
    SoundTrack *track,
    const SoundSampler *sampler,
    int rate);

void
SoundChannel_Pitch(
    SoundChannel *channel,
    unsigned int frequency);

unsigned int
SoundChannel_Fill(
    SoundChannel *channel,
    int *buffer,
    unsigned int size);

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
