
#include <sound.h>

// frequences of octave zero as 24.8 fixed point integers
static const int Octave[] = {
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

static int
NoteSample_Get(
    void *self,
    unsigned int index)
{
  NoteSample *sample = self;
  Note note = sample->note;

  if (index > sample->length || note > length(Octave)) {
    return 0;
  }

  const int pi = 256;
  const int octave = sample->octave;

  /* Note: multiplying frequency (24.8 fixed point) with index (for example a 19.13 fixed
   * point for when using 8kHz as the sample rate) results in a 11.21 fixed point number.
   * This is why we have to shift by 21 to get the alpha/integer part that is used to get
   * the sin-value.
   */
  int frequency = Octave[note] * (1 << octave);
  int alpha = (pi * frequency * index) >> (8 + sample->rate);

  // returns a 24.8 fixed point integer
  int value = Math_sin(alpha);

  // convert to sample size of 8 bits (range -127..+127)
  return (value * 127) >> 8;
}

static inline NoteSample*
NoteSample_FromSamples(union Samples *samples) {
  NoteSample *sample = &samples->note;
  sample->super.self = sample;
  sample->super.Get = NoteSample_Get;
  return sample;
}

static inline bool
NoteSoundChannel_NextSample(NoteSoundChannel *channel) {
  unsigned int index = channel->track.index;
  char *notes = channel->track.notes;

  char symbol = notes[index];
  if (symbol == '\0' || symbol < 'A' || symbol > 'Z') {
    return false;
  }

  enum Note note = (symbol - 'A') * 2;

  unsigned int octave = 4;
  unsigned int dotted = 0;
  unsigned int length = channel->tempo;
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

  // FIXME improve this
  channel->position -= channel->samples.note.length << 12;

  NoteSample *sample = NoteSample_FromSamples(&channel->samples);
  sample->note = note;
  sample->octave = octave;
  sample->length = length;
  sample->rate = channel->rate;

  channel->track.index = index;
  channel->sample = &sample->super;

  return true;
}

SoundChannel*
NoteSoundChannel_Create(
    NoteSoundChannel *channel,
    char *notes,
    int rate)
{
  void NoteSoundChannel_Pitch(void *channel, unsigned int frequency);
  unsigned int NoteSoundChannel_Fill(void *channel, int *buffer, unsigned int size);

  channel->super.self = channel;
  channel->super.Pitch = NoteSoundChannel_Pitch;
  channel->super.Fill = NoteSoundChannel_Fill;

  channel->track.notes = notes;
  channel->track.index = 0;
  channel->rate = rate;
  channel->tempo = 2500;
  channel->position = 0;
  channel->increment = 1 << 12;

  NoteSoundChannel_NextSample(channel);

  return &channel->super;
}

void
NoteSoundChannel_Pitch(
    void *self,
    unsigned int frequency)
{
  NoteSoundChannel *channel = self;

  unsigned int increment = Math_div(1 << (channel->rate + 12), frequency);
  channel->increment = increment;
}

unsigned int
NoteSoundChannel_Fill(
    void *self,
    int *buffer,
    unsigned int size)
{
  NoteSoundChannel *channel = self;
  NoteSample *sample = &channel->samples.note;

  unsigned int position = channel->position >> 12;
  if (position >= sample->length) {
    if (!NoteSoundChannel_NextSample(channel)) {
      return 0; // end of track
    }
  }

  unsigned int index = 0;
  while (index < size) {
    unsigned int position = channel->position >> 12;
    int value = Sample_Get(channel->sample, position);

    channel->position += channel->increment;
    buffer[index++] += value;

    if (position < sample->length) {
      continue;
    }

    // advance to next note/sample
    if (!NoteSoundChannel_NextSample(channel)) {
      break;
    }
  }

  return index;
}

SoundPlayer*
SoundPlayer_GetInstance() {
  static SoundPlayer player = {0};
  return &player;
}

void
SoundPlayer_Enable(SoundPlayer *player) {
  const unsigned int frequency = 11468;
  const unsigned int size = 192;

  static char buffer1[192];
  static char buffer2[192];

  player->buffers[0] = buffer1;
  player->buffers[1] = buffer2;
  player->frequency = frequency;
  player->active = buffer1;
  player->size = size;

  GBA_EnableSound();

  GBA_SoundControl enable = {
    .soundARatio = 1,  // volume at 100%
    .soundAEnable = 3, // left and right speaker
    .soundATimer = 0,  // use timer 0
    .soundAReset = 1,
  };

  GBA_TimerData data = { 0xFFFF - Math_div(16777216, frequency) };
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
  SoundChannel_Pitch(channel, player->frequency);

  for (unsigned int i = 0; i < length(player->channels); i++) {
    if (player->channels[i] == NULL) {
      player->channels[i] = channel;
      return true;
    }
  }

  return false;
}

void
SoundPlayer_MixChannels(SoundPlayer *player) {
  const unsigned int size = player->size;

  int buffer[size];
  GBA_Memset32(buffer, 0, size * sizeof(int));

  for (unsigned int i = 0; i < length(player->channels); i++) {
    SoundChannel *channel = player->channels[i];
    if (channel == NULL) {
      break;
    }

    SoundChannel_Fill(channel, buffer, size);
  }

  for (unsigned int i = 0; i < size; i++) {
    player->active[i] = (char) (buffer[i] >> 2); // divide by 4 channels
  }
}

void
SoundPlayer_VSync(SoundPlayer *player) {
  char *buffer1 = player->buffers[0];
  char *buffer2 = player->buffers[1];

  GBA_System *system = GBA_GetSystem();
  GBA_DirectMemcpy *dma1 = system->directMemcpy[1];

  GBA_DirectMemcpy copy = {0};
  copy.chunkSize = 1; // copy words
  copy.dstAdjust = 2; // fixed destination address
  copy.repeat = 1; // copy at VBlank
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
