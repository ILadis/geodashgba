
#include <sound.h>

void
ModuleSoundChannel_AddSampler(
    ModuleSoundChannel *channel,
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
ModuleSoundChannel_Pitch(
    ModuleSoundChannel *channel,
    unsigned int frequency)
{
  unsigned int reciproc = *channel->base.reciproc;
  unsigned int increment = (frequency * reciproc) >> (SOUND_RECIPROC_PRECISION - SOUND_CHANNEL_PRECISION);
  channel->increment = increment;
}

static inline void
ModuleSoundChannel_SetVolume(
    ModuleSoundChannel *channel,
    unsigned int volume)
{
  const unsigned char max = 1 << SOUND_VOLUME_PRECISION;
  channel->volume = volume > max ? max : volume;
}

static inline void
ModuleSoundChannel_SetSpeed(
    ModuleSoundChannel *channel,
    unsigned char speed)
{
  channel->speed = speed;
}

static inline void
ModuleSoundChannel_SetTempo(
    ModuleSoundChannel *channel,
    unsigned char tempo)
{
  unsigned int frequency = Math_div(tempo * 2, 5);
  unsigned int samples = Math_div(*channel->base.frequency, frequency);
  channel->samplesPerTick = samples;
}

static void
SoundEffect_Arpeggio(
    const Tone *tone,
    ModuleSoundChannel *channel)
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
  ModuleSoundChannel_Pitch(channel, frequency);
}

static void
SoundEffect_PortaUp(
    const Tone *tone,
    ModuleSoundChannel *channel)
{
  unsigned char param = tone->effect.param;

  unsigned int frequency = SoundSampler_GetFrequency(channel->sampler, tone);
  frequency += param * channel->ticks;

  ModuleSoundChannel_Pitch(channel, frequency);
}

static void
SoundEffect_PortaDown(
    const Tone *tone,
    ModuleSoundChannel *channel)
{
  unsigned char param = tone->effect.param;

  unsigned int frequency = SoundSampler_GetFrequency(channel->sampler, tone);
  frequency -= param * channel->ticks;

  ModuleSoundChannel_Pitch(channel, frequency);
}

static void
SoundEffect_SetOffset(
    const Tone *tone,
    ModuleSoundChannel *channel)
{
  unsigned char param = tone->effect.param;
  channel->position = (param * 256) << SOUND_CHANNEL_PRECISION;
}

static void
SoundEffect_SlideVolume(
    const Tone *tone,
    ModuleSoundChannel *channel)
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
    ModuleSoundChannel *channel)
{
  unsigned char param = tone->effect.param;
  ModuleSoundChannel_SetVolume(channel, param);
}

static void
SoundEffect_SetTempo(
    const Tone *tone,
    ModuleSoundChannel *channel)
{
  unsigned char param = tone->effect.param;

  if (param < 32) {
    ModuleSoundChannel_SetSpeed(channel, param);
  } else {
    ModuleSoundChannel_SetTempo(channel, param);
  }
}

static void
SoundEffect_JumptoOrder(
    const Tone *tone,
    ModuleSoundChannel *channel)
{
  ModuleSoundTrack *track = channel->track->self;

  // FIXME must happen to every channel/track
  unsigned char param = tone->effect.param;
  track->position = param * 64;
}

static void
SoundEffect_BreaktoRow(
    const Tone *tone,
    ModuleSoundChannel *channel)
{
  ModuleSoundTrack *track = channel->track->self;

  // FIXME must happen to every channel/track
  unsigned char param = tone->effect.param;
  track->position = ((track->position + 64) & ~63) + param;
}

static inline void
ModuleSoundChannel_ApplySoundEffect(ModuleSoundChannel *channel) {
  typedef void (*SoundEffectFn)(const Tone *tone, ModuleSoundChannel *channel);

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
ModuleSoundChannel_TickTone(ModuleSoundChannel *channel) {
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

    if (sampler != NULL) {
      channel->sampler = sampler;

      // volume is reset when (new) sample is given
      unsigned char volume = SoundSampler_GetVolume(sampler);
      ModuleSoundChannel_SetVolume(channel, volume);
    }

    // if there is no note keep on playing previous one (do not reset position)
    if (tone->note != NOTE_PAUSE) {
      channel->position = 0;

      unsigned int frequency = SoundSampler_GetFrequency(sampler, tone);
      ModuleSoundChannel_Pitch(channel, frequency);
    }
  }

  ModuleSoundChannel_ApplySoundEffect(channel);
  return true;
}

static inline int*
ModuleSoundChannel_FillBuffer(
    ModuleSoundChannel *channel,
    int *buffer, const unsigned int size)
{
  const unsigned char volume = channel->volume;
  unsigned int index = 0;

  const SoundSampler *null = NullSoundSampler_GetInstance();
  const SoundSampler *sampler = channel->sampler == NULL ? null : channel->sampler;

  while (index < size) {
    const unsigned int position = channel->position >> SOUND_CHANNEL_PRECISION;

    int value = SoundSampler_GetSample(sampler, position);
    buffer[index++] += (value * volume) >> SOUND_VOLUME_PRECISION;

    channel->position += channel->increment;
  }

  return &buffer[index];
}

static unsigned int
ModuleSoundChannel_Fill(
    void *self, int *buffer,
    const unsigned int size)
{
  ModuleSoundChannel *channel = self;
  unsigned int remaining = size;

  while (remaining > 0) {
    if (channel->samplesUntilTick == 0) {
      if (!ModuleSoundChannel_TickTone(channel)) {
        break; // reached end of track
      }
      channel->samplesUntilTick = channel->samplesPerTick;
    }

    if (channel->samplesUntilTick < remaining) {
      buffer = ModuleSoundChannel_FillBuffer(channel, buffer, channel->samplesUntilTick);
      remaining -= channel->samplesUntilTick;
      channel->samplesUntilTick = 0;
    }
    else {
      buffer = ModuleSoundChannel_FillBuffer(channel, buffer, remaining);
      channel->samplesUntilTick -= remaining;
      remaining = 0;
    }
  }

  return (size - remaining);
}

static unsigned int
ModuleSoundChannel_SetupAndFill(
    void *self, int *buffer,
    const unsigned int size)
{
  ModuleSoundChannel *channel = self;
  channel->base.Fill = ModuleSoundChannel_Fill;

  // set default speed and tempo
  ModuleSoundChannel_SetSpeed(channel, 6);
  ModuleSoundChannel_SetTempo(channel, 125);

  return ModuleSoundChannel_Fill(self, buffer, size);
}

SoundChannel*
ModuleSoundChannel_ForTrack(
    ModuleSoundChannel *channel,
    SoundTrack *track)
{
  channel->track = track;
  channel->tone = NULL;
  channel->sampler = NULL;
  channel->increment = 0,
  channel->position = 0;
  channel->volume = 1 << SOUND_VOLUME_PRECISION;

  channel->base.self = channel;
  channel->base.Fill = ModuleSoundChannel_SetupAndFill;

  return &channel->base;
}
