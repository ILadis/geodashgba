
#include <sound.h>

// frequences of octave zero as 24.8 fixed point integers
static const unsigned int Octave[] = {
  [NOTE_C]      = 16.3516 * (1 << 8),
  [NOTE_CSHARP] = 17.3239 * (1 << 8),
  [NOTE_D]      = 18.3540 * (1 << 8),
  [NOTE_DSHARP] = 19.4454 * (1 << 8),
  [NOTE_E]      = 20.6017 * (1 << 8),
  [NOTE_F]      = 21.8268 * (1 << 8),
  [NOTE_FSHARP] = 23.1247 * (1 << 8),
  [NOTE_G]      = 24.4997 * (1 << 8),
  [NOTE_GSHARP] = 25.9565 * (1 << 8),
  [NOTE_A]      = 27.5000 * (1 << 8),
  [NOTE_ASHARP] = 29.1352 * (1 << 8),
  [NOTE_B]      = 30.8677 * (1 << 8),
};

unsigned int
Tone_GetFrequency(const Tone *tone) {
  return (tone->note > length(Octave)) ? 0 : Octave[tone->note] * (1 << tone->octave);
}

static const enum Note notes[] = {
  [ 4] = NOTE_C,
         NOTE_CSHARP,
  [ 6] = NOTE_D,
         NOTE_DSHARP,
  [ 8] = NOTE_E,
  [10] = NOTE_F,
         NOTE_FSHARP,
  [12] = NOTE_G,
         NOTE_GSHARP,
  [ 0] = NOTE_A,
         NOTE_ASHARP,
  [ 2] = NOTE_B,
};

static inline bool
Note_FromSymbol(
    Note *note,
    char symbol)
{
  // pause symbol
  if (symbol == 'Z') {
    *note = NOTE_PAUSE;
    return true;
  }

  unsigned int index = (symbol - 'A') * 2;
  if (index > length(notes)) {
    return false;
  }

  *note = notes[index];
  return true;
}

static const Tone*
AsciiSoundTrack_NextTone(void *self) {
  AsciiSoundTrack *track = self;

  unsigned int index = track->index;
  const char *notes = track->notes;

  enum Note note;
  char symbol = notes[index];
  if (!Note_FromSymbol(&note, symbol)) {
    return false;
  }

  unsigned int octave = 4;
  unsigned int dotted = 0;
  unsigned int length = track->tempo;
  bool shorten = false;

  while (true) {
    char modifier = notes[++index];
    if (modifier == ' ' || modifier == '|') {
      continue;
    }

    switch (modifier) {
      case '^': note += 1; continue;
      case '_': note -= 1; continue;
      case '`': octave += 1; continue;
      case ',': octave -= 1; continue;
      case '/': shorten = true; continue;
      case '2': length = shorten ? length >> 1 : length << 1; continue;
      case '4': length = shorten ? length >> 2 : length << 2; continue;
      case '8': length = shorten ? length >> 3 : length << 3; continue;
      case '.': length += length >> ++dotted; continue;
    }

    break;
  }

  Tone *tone = &track->tone;
  tone->note = note;
  tone->octave = octave;
  tone->length = length;
  track->index = index;

  return tone;
}

SoundTrack*
AsciiSoundTrack_FromNotes(
    AsciiSoundTrack *track,
    const char *notes,
    unsigned int tempo)
{
  track->notes = notes;
  track->index = 0;
  track->tempo = tempo;

  track->base.self = track;
  track->base.Next = AsciiSoundTrack_NextTone;

  return &track->base;
}

static void
SineSoundSampler_Prepare(
    unused void *self,
    SoundChannel *channel,
    const Tone *tone)
{
  const int pi2 = 256;

  unsigned int frequency = Tone_GetFrequency(tone); // returns a 24.8 fixed point integer
  unsigned int alpha = (pi2 * frequency) >> 8;

  SoundChannel_Pitch(channel, alpha);
}

static int
SineSoundSampler_GetSample(
    unused void *self,
    unsigned int index)
{
  int value = Math_sin(index);

  // convert to sample size of 8 bits (range -127..+127)
  return (value * 127) >> 8;
}

const SoundSampler*
SineSoundSampler_GetInstance() {
  static SoundSampler sampler = {
    .self = NULL,
    .Prepare = SineSoundSampler_Prepare,
    .Get = SineSoundSampler_GetSample,
  };

  return &sampler;
}

void
SoundChannel_SetTrackAndSampler(
    SoundChannel *channel,
    SoundTrack *track,
    const SoundSampler *sampler)
{
  channel->track = track;
  channel->sampler = sampler;
  channel->position = 0;
  channel->increment = 1 << SOUND_CHANNEL_PRECISION;
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

static inline bool
SoundChannel_NextToneIfRequired(SoundChannel *channel) {
  const Tone *tone = channel->tone;

  const unsigned int position = channel->tonePosition;
  const unsigned int length = tone == NULL ? 0 : tone->length;

  if (position >= length) {
    tone = SoundTrack_NextTone(channel->track);

    if (tone == NULL) {
      return false; // end of track
    }

    channel->tone = tone;
    channel->position = 0;
    channel->tonePosition = 0;

    if (tone->note != NOTE_PAUSE) {
      SoundSampler_Prepare(channel->sampler, channel, tone);
    }
  }

  return true;
}

unsigned int
SoundChannel_Fill(
    SoundChannel *channel,
    int *buffer,
    unsigned int size)
{
  if (!SoundChannel_NextToneIfRequired(channel)) {
    return 0; // end of track
  }

  unsigned int index = 0;
  while (index < size) {
    const unsigned int position = channel->position >> SOUND_CHANNEL_PRECISION;
    const Tone *tone = channel->tone;

    int value = 0;
    if (tone->note != NOTE_PAUSE) {
      value = SoundSampler_GetSample(channel->sampler, position);
    }

    channel->position += channel->increment;
    buffer[index++] += value;

  //const unsigned int next = channel->position >> SOUND_CHANNEL_PRECISION;
    unsigned int next = channel->tonePosition += 1;
    if (next < tone->length) {
      continue;
    }

    // advance to next note/sample
    if (!SoundChannel_NextToneIfRequired(channel)) {
      break;
    }
  }

  return index;
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
    .reciproc = (1 << SOUND_RECIPROC_PRECISION) / 11468,
  };

  return &player;
}

void
SoundPlayer_Enable(unused SoundPlayer *player) {
  GBA_EnableSound();

  GBA_SoundControl enable = {
    .soundARatio = 1,  // volume at 100%
    .soundAEnable = 3, // left and right speaker
    .soundATimer = 0,  // use timer 0
    .soundAReset = 1,
  };

  const int cycles = 16777216;
  const int frequency = 11468;

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
  copy.chunkSize = 1;  // copy words
  copy.dstAdjust = 2;  // fixed destination address
  copy.repeat = 1;     // copy at VBlank
  copy.timingMode = 3; // fifo mode
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
