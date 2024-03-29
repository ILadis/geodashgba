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
  unsigned int length;
  // TODO consider adding effects here
} Tone;

unsigned int
Tone_GetFrequency(const Tone *tone);

static inline int
Tone_GetBaseFrequency() {
  const Tone base = {
    .note = NOTE_C,
    .octave = 4,
  };

  return Tone_GetFrequency(&base);
}

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
  unsigned int tempo; // base used for calculating length of tones (total values that can be sampled)
  Tone tone;          // current parsed note from sound track
} AsciiSoundTrack;

SoundTrack*
AsciiSoundTrack_FromNotes(
    AsciiSoundTrack *track,
    const char *notes,
    unsigned int tempo);

typedef struct SoundChannel SoundChannel;

typedef struct SoundSampler {
  void *self;
  void (*Prepare)(void *self, SoundChannel *channel, const Tone *tone);
  int (*Get)(void *self, unsigned int index);
} SoundSampler;

static inline void
SoundSampler_Prepare(const SoundSampler *sampler, SoundChannel *channel, const Tone *tone) {
  sampler->Prepare(sampler->self, channel, tone);
}

static inline int
SoundSampler_GetSample(const SoundSampler *sampler, unsigned int index) {
  return sampler->Get(sampler->self, index);
}

const SoundSampler*
SineSoundSampler_GetInstance();

typedef struct WaveSoundSampler {
  SoundSampler base;
  Reader *reader;
  unsigned int offset;
  struct {
    unsigned int rate;   // sample rate in Hz
    unsigned short size; // sample size in bytes
  } sample;
} WaveSoundSampler;

SoundSampler*
WaveSoundSampler_FromReader(
    WaveSoundSampler *wave,
    Reader *reader);

typedef struct ModuleTrack {
  Reader *reader;
  unsigned char numChannels; // number of channels in this track
  unsigned char numPatterns; // number of patterns in this track
  unsigned char length;      // length of this track (number of orders in this track)
  unsigned char orders[128];

  struct ModuleChannel {
    struct ModuleTrack *module;
    struct SoundTrack track;
    struct SoundSampler sampler;
    struct Tone tone;      // current parsed note from sound track
    unsigned char number;  // number of this channel in sound track
    unsigned int position; // current position of playback
    unsigned int sample;
  } channels[4];

  struct ModuleSample {
    unsigned int data; // TODO find better name for this
    unsigned char finetune;
    unsigned short length;
    unsigned char volume;
    struct {
      unsigned short start;
      unsigned short length;
    } loop;
  } samples[31];
} ModuleTrack;

typedef struct ModuleChannel ModuleChannel;
typedef struct ModuleSample ModuleSample;

bool
ModuleTrack_FromReader(
    ModuleTrack *track,
    Reader *reader);

SoundTrack*
ModuleTrack_GetSoundTrack(
    ModuleTrack *track,
    unsigned int channel);

SoundSampler*
ModuleTrack_GetSoundSampler(
    ModuleTrack *track,
    unsigned int channel);

typedef struct SoundChannel {
  SoundTrack *track;
  const SoundSampler *sampler;
  const Tone *tone;
  const unsigned int *reciproc; // points to inverse of player frequency (4.28 fixed point integer)
  unsigned int position;        // position in current sample (20.12 fixed point integer)
  unsigned int increment;       // position increment per loop (20.12 fixed point integer)

  unsigned int tonePosition;
} SoundChannel;

// 20.12 fixed point integer (for both increment and position)
#define SOUND_CHANNEL_PRECISION 12
#define SOUND_RECIPROC_PRECISION 24

void
SoundChannel_SetTrackAndSampler(
    SoundChannel *channel,
    SoundTrack *track,
    const SoundSampler *sampler);

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
  unsigned int reciproc;  // holds inverse of player frequency (4.28 fixed point integer)
  struct SoundChannel *channels[4];
} SoundPlayer;

SoundPlayer*
SoundPlayer_GetInstance();

void
SoundPlayer_Enable(SoundPlayer *player);

static inline void
SoundPlayer_SetFrequency(SoundPlayer *player, unsigned int frequency) {
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
