
#include <sound.h>

void
TrackerSoundChannel_AddSampler(
    TrackerSoundChannel *channel,
    const SoundSampler *sampler)
{
  for (unsigned int i = 0; i < length(channel->samplers); i++) {
    if (channel->samplers[i] == NULL) {
      channel->samplers[i] = sampler;
      break;
    }
  }
}

static inline void
TrackerSoundChannel_Pitch(
    TrackerSoundChannel *channel,
    unsigned int frequency)
{
  unsigned int reciproc = *channel->base.reciproc;
  unsigned int increment = (frequency * reciproc) >> (SOUND_RECIPROC_PRECISION - SOUND_CHANNEL_PRECISION);
  channel->increment = increment;
}

static inline void
TrackerSoundChannel_SetVolume(
    TrackerSoundChannel *channel,
    unsigned int volume)
{
  const unsigned char max = 1 << SOUND_VOLUME_PRECISION;
  channel->volume = volume > max ? max : volume;
}

static inline void
TrackerSoundChannel_SetSpeed(
    TrackerSoundChannel *channel,
    unsigned char speed)
{
  channel->speed = speed;
}

static inline void
TrackerSoundChannel_SetTempo(
    TrackerSoundChannel *channel,
    unsigned char tempo)
{
  unsigned int frequency = Math_div(tempo * 2, 5);
  unsigned int samples = Math_div(*channel->base.frequency, frequency);
  channel->samplesPerTick = samples;
}

static void
SoundEffect_Arpeggio(
    const Tone *tone,
    TrackerSoundChannel *channel)
{
  unsigned char param = tone->effect.param;

  unsigned char noteX = (param & 0b11110000) >> 4;
  unsigned char noteY = (param & 0b00001111) >> 0;

  // FIXME steps are actually repeating after every 3 ticks
  unsigned int step = channel->ticks & 0x3; // modulo 4

  Tone next = {
    .octave = tone->octave,
    .note = tone->note,
  };

  switch (step) {
  case 0: // play original note
    break;
  case 1: // play original note + X
    next.note += noteX;
    break;
  case 2: // play original note + Y
    next.note += noteY;
    break;
  }

  unsigned int frequency = SoundSampler_GetFrequency(channel->sampler, tone);
  TrackerSoundChannel_Pitch(channel, frequency);
}

static void
SoundEffect_PortaUp(
    const Tone *tone,
    TrackerSoundChannel *channel)
{
  unsigned char param = tone->effect.param;

  unsigned int frequency = SoundSampler_GetFrequency(channel->sampler, tone);
  frequency += param * channel->ticks;

  TrackerSoundChannel_Pitch(channel, frequency);
}

static void
SoundEffect_PortaDown(
    const Tone *tone,
    TrackerSoundChannel *channel)
{
  unsigned char param = tone->effect.param;

  unsigned int frequency = SoundSampler_GetFrequency(channel->sampler, tone);
  frequency -= param * channel->ticks;

  TrackerSoundChannel_Pitch(channel, frequency);
}

static void
SoundEffect_SetOffset(
    const Tone *tone,
    TrackerSoundChannel *channel)
{
  unsigned char param = tone->effect.param;
  channel->position = (param * 256) << SOUND_CHANNEL_PRECISION;
}

static void
SoundEffect_SlideVolume(
    const Tone *tone,
    TrackerSoundChannel *channel)
{
  unsigned char param = tone->effect.param;

  unsigned char increase = (param & 0b11110000) >> 4;
  unsigned char decrease = (param & 0b00001111) >> 0;

  unsigned char volume = channel->volume + increase - decrease;

  const unsigned char max = 1 << SOUND_VOLUME_PRECISION;
  // clamp new volume (if going below zero or above maximum)
  if (decrease && volume > max) volume = 0;
  if (increase && volume > max) volume = max;

  channel->volume = volume;
}

static void
SoundEffect_SetVolume(
    const Tone *tone,
    TrackerSoundChannel *channel)
{
  unsigned char param = tone->effect.param;
  TrackerSoundChannel_SetVolume(channel, param);
}

static void
SoundEffect_SetTempo(
    const Tone *tone,
    TrackerSoundChannel *channel)
{
  unsigned char param = tone->effect.param;

  if (param < 32) {
    TrackerSoundChannel_SetSpeed(channel, param);
  } else {
    TrackerSoundChannel_SetTempo(channel, param);
  }
}

static void
SoundEffect_JumptoOrder(
    const Tone *tone,
    TrackerSoundChannel *channel)
{
  // FIXME must happen to every channel/track
  unsigned char param = tone->effect.param;
  unsigned int position = param * 64;

  SoundTrack_SeekTo(channel->track, position);
}

static void
SoundEffect_BreaktoRow(
    const Tone *tone,
    TrackerSoundChannel *channel)
{
  // FIXME should not rely on a specific sound track 
  ModuleSoundTrack *track = channel->track->self;

  // FIXME must happen to every channel/track
  unsigned char param = tone->effect.param;
  unsigned int position = ((track->position + 64) & ~63) + param;

  SoundTrack_SeekTo(channel->track, position);
}

static inline void
TrackerSoundChannel_ApplySoundEffect(TrackerSoundChannel *channel) {
  typedef void (*SoundEffectFn)(const Tone *tone, TrackerSoundChannel *channel);

  static const SoundEffectFn entry[SOUND_EFFECT_COUNT] = {
    [SOUND_EFFECT_SET_OFFSET]   = SoundEffect_SetOffset,
    [SOUND_EFFECT_SET_VOLUME]   = SoundEffect_SetVolume,
    [SOUND_EFFECT_SET_TEMPO]    = SoundEffect_SetTempo,
    [SOUND_EFFECT_JUMPTO_ORDER] = SoundEffect_JumptoOrder,
    [SOUND_EFFECT_BREAKTO_ROW]  = SoundEffect_BreaktoRow,
  };

  static const SoundEffectFn update[SOUND_EFFECT_COUNT] = {
    [SOUND_EFFECT_ARPEGGIO]     = SoundEffect_Arpeggio,
    [SOUND_EFFECT_PORTA_UP]     = SoundEffect_PortaUp,
    [SOUND_EFFECT_PORTA_DOWN]   = SoundEffect_PortaDown,
    [SOUND_EFFECT_VOLUME_SLIDE] = SoundEffect_SlideVolume,
  };

  const Tone *tone = channel->tone;
  const SoundEffectFn *effects = channel->ticks == 0 ? entry : update;

  SoundEffect effect = tone->effect;
  SoundEffectFn handler = effects[effect.type];

  if (effect.type != 0 && effect.param != 0 && handler != NULL) {
    handler(tone, channel);
  }
}

static inline bool
TrackerSoundChannel_TickTone(TrackerSoundChannel *channel) {
  const Tone *tone = channel->tone;
  unsigned char ticks = channel->ticks++;

  // FIXME should consider tone->ticks (i.e. length of note)
  bool advance = tone == NULL || ticks >= channel->speed;
  if (advance) {
    tone = SoundTrack_NextTone(channel->track);

    channel->tone = tone;
    channel->ticks = 0;

    if (tone == NULL) {
      // reached end of track
      return false;
    }

    // calculate sample index by modulo 32 (since max number of samplers is 31)
    unsigned int index = tone->sample & length(channel->samplers);
    const SoundSampler *sampler = channel->samplers[index];

    // FIXME improve check for "no sample" (where sample index == -1)
    if (sampler != NULL && tone->sample < 32) {
      channel->sampler = sampler;

      // volume is reset when (new) sample is given
      unsigned char volume = SoundSampler_GetVolume(sampler);
      TrackerSoundChannel_SetVolume(channel, volume);
    }

    // if there is no note keep on playing previous one (do not reset position)
    if (tone->note != NOTE_PAUSE) {
      channel->position = 0;

      unsigned int frequency = SoundSampler_GetFrequency(channel->sampler, tone);
      TrackerSoundChannel_Pitch(channel, frequency);
    }
  }

  TrackerSoundChannel_ApplySoundEffect(channel);
  return true;
}

static inline int*
TrackerSoundChannel_FillBuffer(
    TrackerSoundChannel *channel,
    int *buffer, const unsigned int size)
{
  const SoundSampler *null = NullSoundSampler_GetInstance();
  const SoundSampler *sampler = channel->sampler == NULL ? null : channel->sampler;
  return SoundSampler_FillBuffer(sampler, buffer, &channel->position, channel->increment, channel->volume, size);
}

static unsigned int
TrackerSoundChannel_Fill(
    void *self, int *buffer,
    const unsigned int size)
{
  TrackerSoundChannel *channel = self;
  unsigned int remaining = size;

  while (remaining > 0) {
    if (channel->samplesUntilTick == 0) {
      if (!TrackerSoundChannel_TickTone(channel)) {
        break; // reached end of track
      }
      channel->samplesUntilTick = channel->samplesPerTick;
    }

    if (channel->samplesUntilTick < remaining) {
      buffer = TrackerSoundChannel_FillBuffer(channel, buffer, channel->samplesUntilTick);
      remaining -= channel->samplesUntilTick;
      channel->samplesUntilTick = 0;
    }
    else {
      buffer = TrackerSoundChannel_FillBuffer(channel, buffer, remaining);
      channel->samplesUntilTick -= remaining;
      remaining = 0;
    }
  }

  return (size - remaining);
}

static unsigned int
TrackerSoundChannel_SetupAndFill(
    void *self, int *buffer,
    const unsigned int size)
{
  TrackerSoundChannel *channel = self;
  channel->base.Fill = TrackerSoundChannel_Fill;

  // set default speed and tempo
  TrackerSoundChannel_SetSpeed(channel, 6);
  TrackerSoundChannel_SetTempo(channel, 125);

  return TrackerSoundChannel_Fill(self, buffer, size);
}

SoundChannel*
TrackerSoundChannel_ForTrack(
    TrackerSoundChannel *channel,
    SoundTrack *track)
{
  channel->track = track;
  channel->tone = NULL;
  channel->sampler = NULL;
  channel->increment = 0,
  channel->position = 0;
  channel->volume = 1 << SOUND_VOLUME_PRECISION;

  channel->base.self = channel;
  channel->base.Fill = TrackerSoundChannel_SetupAndFill;

  return &channel->base;
}
