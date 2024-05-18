
#include <sound.h>

void
SoundChannel_AddSampler(
    SoundChannel *channel,
    const SoundSampler *sampler)
{
  for (unsigned int i = 0; i < length(channel->samplers); i++) {
    if (channel->samplers[i] == NULL) {
      channel->samplers[i] = sampler;
      break;
    }
  }
}

void
SoundChannel_Pitch(
    SoundChannel *channel,
    unsigned int frequency)
{
  unsigned int reciproc = *channel->reciproc;
  unsigned int increment = (frequency * reciproc) >> (SOUND_RECIPROC_PRECISION - SOUND_CHANNEL_PRECISION);
  channel->increment = increment;
}

void
SoundChannel_SetTempo(
    SoundChannel *channel,
    unsigned char tempo)
{
  unsigned int frequency = Math_div(tempo * 2, 5);
  unsigned int samples = Math_div(*channel->frequency, frequency);
  channel->samplesPerTick = samples;
}

static void
SoundEffect_Arpeggio(
    const Tone *tone,
    SoundChannel *channel)
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
  SoundChannel_Pitch(channel, frequency);
}

static void
SoundEffect_PortaUp(
    const Tone *tone,
    SoundChannel *channel)
{
  unsigned char param = tone->effect.param;

  unsigned int frequency = SoundSampler_GetFrequency(channel->sampler, tone);
  frequency += param * channel->ticks;

  SoundChannel_Pitch(channel, frequency);
}

static void
SoundEffect_PortaDown(
    const Tone *tone,
    SoundChannel *channel)
{
  unsigned char param = tone->effect.param;

  unsigned int frequency = SoundSampler_GetFrequency(channel->sampler, tone);
  frequency -= param * channel->ticks;

  SoundChannel_Pitch(channel, frequency);
}

static void
SoundEffect_SetOffset(
    const Tone *tone,
    SoundChannel *channel)
{
  unsigned char param = tone->effect.param;
  channel->position = (param * 256) << SOUND_CHANNEL_PRECISION;
}

static void
SoundEffect_SlideVolume(
    const Tone *tone,
    SoundChannel *channel)
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
    SoundChannel *channel)
{
  unsigned char param = tone->effect.param;
  SoundChannel_SetVolume(channel, param);
}

static void
SoundEffect_SetTempo(
    const Tone *tone,
    SoundChannel *channel)
{
  unsigned char param = tone->effect.param;

  if (param < 32) {
    SoundChannel_SetSpeed(channel, param);
  } else {
    SoundChannel_SetTempo(channel, param);
  }
}

static void
SoundEffect_JumptoOrder(
    const Tone *tone,
    SoundChannel *channel)
{
  ModuleSoundTrack *track = channel->track->self;

  // FIXME must happen to every channel/track
  unsigned char param = tone->effect.param;
  track->position = param * 64;
}

static void
SoundEffect_BreaktoRow(
    const Tone *tone,
    SoundChannel *channel)
{
  ModuleSoundTrack *track = channel->track->self;

  // FIXME must happen to every channel/track
  unsigned char param = tone->effect.param;
  track->position = ((track->position + 64) & ~63) + param;
}

static void
SoundEffect_ApplyFor(
    const Tone *tone,
    SoundChannel *channel)
{
  static const SoundEffectFn entry[SOUND_EFFECT_COUNT] = {
    [SOUND_EFFECT_SET_OFFSET]   = SoundEffect_SetOffset,
    [SOUND_EFFECT_SET_VOLUME]   = SoundEffect_SetVolume,
    [SOUND_EFFECT_SET_TEMPO]    = SoundEffect_SetTempo,
    [SOUND_EFFECT_JUMPTO_ORDER] = SoundEffect_JumptoOrder,
    [SOUND_EFFECT_BREAKTO_ROW]  = SoundEffect_BreaktoRow,
  };

  static const SoundEffectFn update[SOUND_EFFECT_COUNT] = {
    [SOUND_EFFECT_ARPEGGIO] = SoundEffect_Arpeggio,
    [SOUND_EFFECT_PORTA_UP] = SoundEffect_PortaUp,
    [SOUND_EFFECT_PORTA_DOWN] = SoundEffect_PortaDown,
    [SOUND_EFFECT_VOLUME_SLIDE] = SoundEffect_SlideVolume,
  };

  const SoundEffectFn *effects = channel->ticks == 0 ? entry : update;

  SoundEffect effect = tone->effect;
  SoundEffectFn handler = effect.type < SOUND_EFFECT_COUNT ? effects[effect.type] : NULL;

  if (effect.type != 0 && effect.param != 0 && handler != NULL) {
    handler(tone, channel);
  }
}

static inline bool
SoundChannel_TickTone(SoundChannel *channel) {
  const Tone *tone = channel->tone;
  unsigned int ticks = ++channel->ticks;

  // FIXME should consider tone->ticks (i.e. length of note)
  bool next = tone == NULL || ticks >= channel->speed;
  if (next) {
    tone = SoundTrack_NextTone(channel->track);

    channel->tone = tone;
    channel->ticks = 0;

    if (tone == NULL) {
      // reached end of track
      return false;
    }

    const SoundSampler *sampler = tone->sample < length(channel->samplers) ? channel->samplers[tone->sample]: NULL;
    if (sampler != NULL) {
      channel->sampler = sampler;

      // volume is reset when (new) sample is given
      unsigned char volume = SoundSampler_GetVolume(sampler);
      SoundChannel_SetVolume(channel, volume);
    }

    // if there is no note keep on playing previous one (do not reset position)
    if (tone->note != NOTE_PAUSE) {
      channel->position = 0;

      unsigned int frequency = SoundSampler_GetFrequency(sampler, tone);
      SoundChannel_Pitch(channel, frequency);
    }
  }

  SoundEffect_ApplyFor(tone, channel);
  return true;
}

static inline int*
SoundChannel_FillBuffer(
    SoundChannel *channel,
    int *buffer,
    unsigned int size)
{
  const unsigned char volume = channel->volume;
  unsigned int index = 0;

  while (index < size) {
    const unsigned int position = channel->position >> SOUND_CHANNEL_PRECISION;

    int value = 0;
    // TODO improve thos (use NullSampler if tone is a pause)
    if (channel->sampler != NULL) {
      value = SoundSampler_GetSample(channel->sampler, position);
    }

    channel->position += channel->increment;
    buffer[index++] += (value * volume) >> SOUND_VOLUME_PRECISION;
  }

  return &buffer[index];
}

unsigned int
SoundChannel_Fill(
    SoundChannel *channel,
    int *buffer,
    const unsigned int size)
{
  unsigned int remaining = size;

  while (remaining > 0) {
    if (channel->samplesUntilTick == 0) {
      if (!SoundChannel_TickTone(channel)) {
        break; // reached end of track
      }
      channel->samplesUntilTick = channel->samplesPerTick;
    }

    if (channel->samplesUntilTick < remaining) {
      buffer = SoundChannel_FillBuffer(channel, buffer, channel->samplesUntilTick);
      remaining -= channel->samplesUntilTick;
      channel->samplesUntilTick = 0;
    }
    else {
      buffer = SoundChannel_FillBuffer(channel, buffer, remaining);
      channel->samplesUntilTick -= remaining;
      remaining = 0;
    }
  }

  return (size - remaining);
}

SoundPlayer*
SoundPlayer_GetInstance() {
  /* Note: at around 60 fps every frame consumes 192 samples each of 1 byte in size. This is
   * determined by the target frequency (11468 samples/s divided by 60 fps). The buffer/sample
   * size also needs to be a multiple of 16 because the sound is read in chunks of 16 bytes.
   */
  static char buffer1[192];
  static char buffer2[192];

  static SoundPlayer player = {
    .active = buffer1,
    .buffers[0] = buffer1,
    .buffers[1] = buffer2,
    .size = length(buffer1),
    .frequency = 11468,
    .reciproc = (1 << SOUND_RECIPROC_PRECISION) / 11468,
  };

  return &player;
}

void
SoundPlayer_Enable(unused SoundPlayer *player) {
  GBA_EnableSound();

  GBA_SoundControl enable = {
    .soundARatio = 1,  // volume at 100%
    .soundAEnable = 3, // use both left and right speaker
    .soundATimer = 0,  // use timer 0 to trigger DMA
    .soundAReset = 1,
  };

  const int cycles = 16777216;
  const int frequency = player->frequency; // default is 11468

  GBA_TimerData data = { 0xFFFF - Math_div(cycles, frequency) };
  GBA_TimerControl timer = {
    .frequency = 0,
    .enable = 1,
  };

  GBA_System *system = GBA_GetSystem();

  GBA_SoundControl *sound = system->soundControl;
  sound->value = enable.value;

  GBA_TimerControl *timer0 = system->timerControl[0];
  timer0->value = timer.value;

  GBA_TimerData *data0 = system->timerData[0];
  data0->value = data.value;
}

bool
SoundPlayer_AddChannel(
    SoundPlayer *player,
    SoundChannel *channel)
{
  for (unsigned int i = 0; i < length(player->channels); i++) {
    if (player->channels[i] == NULL) {
      player->channels[i] = channel;
      channel->frequency = &player->frequency;
      channel->reciproc = &player->reciproc;
      return true;
    }
  }

  return false;
}

bool
SoundPlayer_MixChannels(SoundPlayer *player) {
  const unsigned int size = player->size;

  int buffer[size];
  GBA_Memset32(buffer, 0, size * sizeof(int));

  bool done = true;
  for (unsigned int i = 0; i < length(player->channels); i++) {
    SoundChannel *channel = player->channels[i];
    if (channel == NULL) {
      break;
    }

    if (SoundChannel_Fill(channel, buffer, size) > 0) {
      done = false;
    }
  }

  for (unsigned int i = 0; i < size; i++) {
    player->active[i] = (char) (buffer[i] >> 2); // divide by 4 channels
  }

  return !done;
}

void
SoundPlayer_VSync(SoundPlayer *player) {
  char *buffer1 = player->buffers[0];
  char *buffer2 = player->buffers[1];

  GBA_System *system = GBA_GetSystem();
  GBA_DirectMemcpy *dma1 = system->directMemcpy[1];

  GBA_DirectMemcpy copy = {0};
  copy.chunkSize = 1;  // copy words (4 bytes at a time)
  copy.srcAdjust = 0;  // increment destination address
  copy.dstAdjust = 2;  // fixed destination address
  copy.timingMode = 3; // sound/fifo mode
  copy.enable = 1;

  if (player->active == buffer1) {
    dma1->cnt = 0;

    dma1->src = buffer1;
    dma1->dst = system->soundDataA;
    dma1->cnt = copy.cnt;

    player->active = buffer2;
  } else {
    dma1->cnt = 0;

    dma1->src = buffer2;
    dma1->dst = system->soundDataA;
    dma1->cnt = copy.cnt;

    player->active = buffer1;
  }
}
