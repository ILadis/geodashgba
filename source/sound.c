
#include <sound.h>

SoundPlayer*
SoundPlayer_GetInstance() {
  /* Note: at around 60 fps every frame consumes 192 samples each of 1 byte in size. This is
   * determined by the target frequency (11468 samples/s divided by 60 fps). The buffer/sample
   * size also needs to be a multiple of 16 because the sound is read in chunks of 16 bytes.
   */
  static char buffer1[304];
  static char buffer2[304];

  static SoundPlayer player = {
    .active = buffer1,
    .buffers[0] = buffer1,
    .buffers[1] = buffer2,
    .size = length(buffer1),
    .frequency = 18158,
    .reciproc = (1 << SOUND_RECIPROC_PRECISION) / 18157,
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
  copy.repeat = 1;     // repeat copy once sound buffer is empty
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

int*
SoundSampler_SlowFillBuffer(
    const SoundSampler *sampler,
    int *buffer,
    unsigned int *position,
    unsigned int increment,
    unsigned char volume,
    unsigned int size)
{
  while (size-- > 0) {
    const unsigned int index = (*position) >> SOUND_CHANNEL_PRECISION;
    int value = SoundSampler_GetSample(sampler, index);

    *buffer++ += (value * volume) >> SOUND_VOLUME_PRECISION;
    *position += increment;
  }

  return buffer;
}
