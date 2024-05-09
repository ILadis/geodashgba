#ifndef SOUND_H
#define SOUND_H

#include <types.h>
#include <math.h>
#include <gba.h>
#include <io.h>

typedef enum Note {
  NOTE_C,
  NOTE_CSHARP,
  NOTE_D,
  NOTE_DSHARP,
  NOTE_E,
  NOTE_F,
  NOTE_FSHARP,
  NOTE_G,
  NOTE_GSHARP,
  NOTE_A,
  NOTE_ASHARP,
  NOTE_B,
} Note;

#define NOTE_PAUSE ((enum Note) 0xFF)

typedef struct Tone {
  Note note;
  unsigned int octave;
  unsigned int sample; // index of sample this tone will use
  unsigned int ticks;  // number of ticks this tone will last (.6 fixed point integer, 64 == one full note)

  // TODO consider adding effects here
} Tone;

#define NOTE_TICKS_PRECISION 6

typedef struct SoundTrack {
  void *self;
  const Tone* (*Next)(void *self);
} SoundTrack;

static inline const Tone*
SoundTrack_NextTone(SoundTrack *track) {
  return track->Next(track->self);
}

typedef struct AsciiSoundTrack {
  SoundTrack base;
  const char *notes;
  unsigned int index; // index of next char to consume in notes pointer
  Tone tone;          // current parsed note from sound track
} AsciiSoundTrack;

SoundTrack*
AsciiSoundTrack_FromNotes(
    AsciiSoundTrack *track,
    const char *notes);

typedef struct ModuleSoundTrack {
  SoundTrack base;
  const Reader *reader;
  unsigned int position; // current position in module track
  unsigned int channel;  // channel number of module track
  Tone tone;             // current parsed note from module track
} ModuleSoundTrack;

SoundTrack*
ModuleSoundTrack_FromReader(
    ModuleSoundTrack *track,
    const Reader *reader,
    unsigned int channel);

typedef struct SoundChannel SoundChannel;

typedef struct SoundSampler {
  void *self;
  int (*Get)(void *self, unsigned int index);
  unsigned int (*Frequency)(void *self, const Tone *tone);
  unsigned char (*Volume)(void *self);
} SoundSampler;

static inline int
SoundSampler_GetSample(const SoundSampler *sampler, unsigned int index) {
  return sampler->Get(sampler->self, index);
}

static inline unsigned int
SoundSampler_GetFrequency(const SoundSampler *sampler, const Tone *tone) {
  return sampler->Frequency(sampler->self, tone);
}

static inline unsigned char
SoundSampler_GetVolume(const SoundSampler *sampler) {
  return sampler->Volume(sampler->self);
}

const SoundSampler*
SineSoundSampler_GetInstance();

typedef struct ModuleSoundSampler {
  SoundSampler base;
  const Reader *reader;
  unsigned int index;
} ModuleSoundSampler;

SoundSampler*
ModuleSoundSampler_FromReader(
    ModuleSoundSampler *sampler,
    const Reader *reader,
    unsigned int index);

// TODO rename to TrackSoundChannel
typedef struct SoundChannel {
  const SoundSampler *sampler;
  const SoundSampler *samplers[32];
  const unsigned int *frequency; // points to sound player frequency
  const unsigned int *reciproc;  // points to inverse of sound player frequency (4.28 fixed point integer)
  unsigned int position;         // position in current sample (20.12 fixed point integer)
  unsigned int increment;        // position increment per loop (20.12 fixed point integer)
  unsigned char volume;          // volume at which this sound channel plays (1.6 fixed point integer)

  SoundTrack *track;
  const Tone *tone;
  unsigned int ticks;            // amount of times tone was already ticked

  unsigned int samplesPerTick;
  unsigned int samplesUntilTick;
} SoundChannel;

// 20.12 fixed point integer (for both increment and position)
#define SOUND_CHANNEL_PRECISION 12
#define SOUND_RECIPROC_PRECISION 24
#define SOUND_VOLUME_PRECISION 6

void
SoundChannel_AssignTrack(
    SoundChannel *channel,
    SoundTrack *track);

void
SoundChannel_AddSampler(
    SoundChannel *channel,
    const SoundSampler *sampler);

void
SoundChannel_Pitch(
    SoundChannel *channel,
    unsigned int frequency);

static inline void
SoundChannel_SetVolume(
    SoundChannel *channel,
    unsigned char volume)
{
  channel->volume = Math_max(volume, 1 << SOUND_VOLUME_PRECISION);
}

void
SoundChannel_SetTempo(
    SoundChannel *channel,
    unsigned int frequency); // the duration one note tick should last (e.g. 5Hz = 0.2 seconds)

unsigned int
SoundChannel_Fill(
    SoundChannel *channel,
    int *buffer,
    unsigned int size);

typedef struct SoundPlayer {
  char *buffers[2];       // buffers for mixing
  char *active;           // currently active buffer
  unsigned int size;      // buffer size
  unsigned int frequency; // current playback frequency (amount of samples to play per second)
  unsigned int reciproc;  // holds inverse of current playback frequency (4.28 fixed point integer)
  struct SoundChannel *channels[4];
} SoundPlayer;

SoundPlayer*
SoundPlayer_GetInstance();

void
SoundPlayer_Enable(SoundPlayer *player);

static inline void
SoundPlayer_SetFrequency(SoundPlayer *player, unsigned int frequency) {
  player->frequency = frequency;
  player->reciproc = Math_div(1 << SOUND_RECIPROC_PRECISION, frequency);
}

bool
SoundPlayer_AddChannel(
    SoundPlayer *player,
    SoundChannel *channel);

bool
SoundPlayer_MixChannels(SoundPlayer *player);

void
SoundPlayer_VSync(SoundPlayer *player);

#endif
