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

#define NOTE_COUNT 12
#define NOTE_PAUSE ((enum Note) 0xFF)

typedef struct SoundEffect {
  enum {
    SOUND_EFFECT_ARPEGGIO     = 0x0,
    SOUND_EFFECT_PORTA_UP     = 0x1,
    SOUND_EFFECT_PORTA_DOWN   = 0x2,
    SOUND_EFFECT_PORTA_TONE   = 0x3,
    // ...
    SOUND_EFFECT_SET_OFFSET   = 0x9,
    SOUND_EFFECT_VOLUME_SLIDE = 0xA,
    SOUND_EFFECT_JUMPTO_ORDER = 0xB,
    SOUND_EFFECT_SET_VOLUME   = 0xC,
    SOUND_EFFECT_BREAKTO_ROW  = 0xD,
    SOUND_EFFECT_SET_TEMPO    = 0xF,
  } type;
  unsigned char param;
} SoundEffect;

#define SOUND_EFFECT_COUNT 16

typedef struct Tone {
  Note note;
  SoundEffect effect;
  unsigned int octave;
  unsigned int sample; // index of sample this tone will use
} Tone;

#define TONE_TICKS_PRECISION 6

typedef struct SoundTrack {
  void *self;
  const Tone* (*Next)(void *self);
  bool (*SeekTo)(void *self, unsigned int position);
} SoundTrack;

static inline const Tone*
SoundTrack_NextTone(SoundTrack *track) {
  return track->Next(track->self);
}

static inline bool
SoundTrack_SeekTo(SoundTrack *track, unsigned int position) {
  return track->SeekTo(track->self, position);
}

typedef struct AsciiSoundTrack {
  SoundTrack base;
  const Reader *reader; // reader that holds the sound track in text form
  Tone tone;            // current parsed note from sound track
} AsciiSoundTrack;

bool
AsciiSoundTrack_To(
    SoundTrack *track,
    const Writer *writer,
    char separator);

SoundTrack*
AsciiSoundTrack_From(
    AsciiSoundTrack *track,
    const Reader *reader);

typedef struct Binv1SoundTrack {
  SoundTrack base;
  unsigned int position, length;
  const Tone *tones;
} Binv1SoundTrack;

// TODO implement Binv1SoundTrackTo(...)

SoundTrack*
Binv1SoundTrack_From(
    Binv1SoundTrack *track,
    // TODO use "const Reader*" instead
    const Tone *tones,
    unsigned int length);

typedef struct ModuleSoundTrack {
  SoundTrack base;
  const Reader *reader;
  unsigned int position; // current position in module track
  unsigned int channel;  // channel number of module track
  Tone tone;             // current parsed note from module track
} ModuleSoundTrack;

SoundTrack*
ModuleSoundTrack_From(
    ModuleSoundTrack *track,
    const Reader *reader,
    unsigned int channel);

typedef struct SoundSampler {
  void *self;
  int (*Get)(void *self, unsigned int index);
  int* (*Fill)(void *self, int *buffer, unsigned int *position, unsigned int increment, unsigned char volume, unsigned int size);
  unsigned int (*Length)(void *self);
  unsigned int (*Frequency)(void *self, const Tone *tone);
  unsigned char (*Volume)(void *self);
} SoundSampler;

static inline int
SoundSampler_GetSample(const SoundSampler *sampler, unsigned int index) {
  return sampler->Get(sampler->self, index);
}

static inline int*
SoundSampler_FillBuffer(const SoundSampler *sampler, int *buffer, unsigned int *position, unsigned int increment, unsigned char volume, unsigned int size) {
  return sampler->Fill(sampler->self, buffer, position, increment, volume, size);
}

static inline unsigned int
SoundSampler_GetLength(const SoundSampler *sampler) {
  return sampler->Length(sampler->self);
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
NullSoundSampler_GetInstance();

const SoundSampler*
SineSoundSampler_GetInstance();

typedef struct Binv1SoundSampler {
  SoundSampler base;
  unsigned char volume;
  const unsigned int *frequencies;
  unsigned int length;
  const int *samples;
} Binv1SoundSampler;

bool
Binv1SoundSampler_To(
    SoundSampler *sampler,
    const Writer *writer);

SoundSampler*
Binv1SoundSampler_From(
    Binv1SoundSampler *sampler,
    const unsigned int *data);

typedef struct ModuleSoundSampler {
  SoundSampler base;
  const Reader *reader;
  unsigned int index;
} ModuleSoundSampler;

SoundSampler*
ModuleSoundSampler_From(
    ModuleSoundSampler *sampler,
    const Reader *reader,
    unsigned int index);

typedef struct WaveSoundSampler {
  SoundSampler base;
  const Reader *reader;
} WaveSoundSampler;

SoundSampler*
WaveSoundSampler_From(
    WaveSoundSampler *sampler,
    const Reader *reader);

typedef struct SoundChannel {
  void *self;
  const unsigned int *frequency; // points to sound player frequency
  const unsigned int *reciproc;  // points to inverse of sound player frequency (4.28 fixed point integer)
  unsigned int (*Fill)(void *self, int *buffer, unsigned int size);
} SoundChannel;

static inline unsigned int
SoundChannel_Fill(SoundChannel *channel, int *buffer, unsigned int size) {
  return channel->Fill(channel->self, buffer, size);
}

typedef struct TrackerSoundChannel {
  SoundChannel base;

  SoundTrack *track;
  const Tone *tone;
  const SoundSampler *sampler;
  const SoundSampler *samplers[31];

  unsigned int position;  // position in current sample (20.12 fixed point integer)
  unsigned int increment; // position increment per loop (20.12 fixed point integer)
  unsigned char volume;   // volume at which this sound channel plays (1.6 fixed point integer)
  unsigned char ticks;    // amount of times tone was already ticked
  unsigned char speed;    // amount of times a tone must be ticked before advancing to next tone

  unsigned int samplesPerTick;
  unsigned int samplesUntilTick;
} TrackerSoundChannel;

// 20.12 fixed point integer (for both increment and position)
#define SOUND_CHANNEL_PRECISION 12
#define SOUND_RECIPROC_PRECISION 24
#define SOUND_VOLUME_PRECISION 6

SoundChannel*
TrackerSoundChannel_ForTrack(
    TrackerSoundChannel *channel,
    SoundTrack *track);

void
TrackerSoundChannel_AddSampler(
    TrackerSoundChannel *channel,
    const SoundSampler *sampler);

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
