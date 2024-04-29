
#include <sound.h>

#define ModuleTrack_SignatureValue(s1, s2, s3, s4) (0 \
    | (s1 <<  0) \
    | (s2 <<  8) \
    | (s3 << 16) \
    | (s4 << 24))

#define ModuleTrack_ValueOf(value) (2 * ( \
    + ((value >> 0) & 0xFF) * 0x100 \
    + ((value >> 8) & 0xFF)))

#define SAMPLES_POSITION   20
#define ORDERS_POSITION    950
#define SIGNATURE_POSITION 1080
#define PATTERN_POSITION   1084

/* Note: these are the amiga period values for the first octave (C-1 to B-1).
 * An amiga period represents the amount of samples that should be played
 * between each amiga vblank.
 */
static const unsigned int Period1[] = {
  [NOTE_C]      = 856,
  [NOTE_CSHARP] = 808,
  [NOTE_D]      = 762,
  [NOTE_DSHARP] = 720,
  [NOTE_E]      = 678,
  [NOTE_F]      = 640,
  [NOTE_FSHARP] = 604,
  [NOTE_G]      = 570,
  [NOTE_GSHARP] = 538,
  [NOTE_A]      = 508,
  [NOTE_ASHARP] = 480,
  [NOTE_B]      = 453,
};

static inline unsigned int
Tone_GetPeriod(const Tone *tone) {
  return (tone->note > length(Period1)) ? 0 : (Period1[tone->note] * 2) >> tone->octave;
}

/* Note: this converts amige periods to samples/s (Hz). This value determines
 * the amount of samples that should be played per second for the given note.
 */
static inline unsigned int
Tone_GetModFrequency(const Tone *tone) {
  // TODO use finetune lookup table
  return 3579545 / Tone_GetPeriod(tone);
}

// finds the closest note for the given amiga period value
static inline void
Tone_FromPeriod(
    Tone *tone,
    unsigned int period)
{
  unsigned int closest = ~0;

  if (period == 0) {
    tone->octave = 4;
    tone->note = NOTE_PAUSE;
  }

  else for (unsigned int octave = 0; octave < 5; octave++) {
    for (enum Note note = 0; note < length(Period1); note++) {
      const Tone current = {
        .note = note,
        .octave = octave,
      };

      unsigned int notePeriod = Tone_GetPeriod(&current);
      unsigned int distance = Math_abs(notePeriod - period);

      if (distance < closest) {
        tone->note = note;
        tone->octave = octave;

        closest = distance;
      }
    }
  }
}

bool
ModuleChannel_ReadCurrentPattern(
    ModuleChannel *channel,
    unsigned char pattern[4])
{
  ModuleTrack *module = channel->module;
  Reader *reader = module->reader;

  int index = channel->position >> 6; // divide position by 64 to get current order index
  unsigned char order = module->orders[index];

  unsigned int row = channel->position & 63; // current row is calculated by modulo 64
  unsigned int channels = module->numChannels;

  // 64 patterns per order and 4 bytes per pattern for each channel
  unsigned int offset = PATTERN_POSITION
    + (channels * 64 * 4 * order)
    + (channels * 4 * row)
    + (channel->number * 4);

  if (!Reader_SeekTo(reader, offset)) {
    return NULL;
  }

  return Reader_ReadValue(reader, pattern, 4);
}

static inline bool
ModuleChannel_HasNextTone(ModuleChannel *channel) {
  /* Note: multiply length of module track (= number of orders) by number of patterns
   * per order (= 64) to get the number of total patterns in this track.
   */
  const unsigned int endpos = channel->module->numOrders * 64;
  return channel->position < endpos;
}

static const Tone*
ModuleChannel_NextTone(void *self) {
  ModuleChannel *channel = self;

  if (!ModuleChannel_HasNextTone(channel)) {
    return NULL;
  }

  unsigned char pattern[4];
  if (!ModuleChannel_ReadCurrentPattern(channel, pattern)) {
    return NULL;
  }

  // TODO add ModulePattern struct to store all fields
  unsigned int period = ((pattern[0] & 0b00001111) << 8) + pattern[1];
  unsigned int sample = (pattern[0] & 0b11110000) + (pattern[2] >> 4);

  Tone *tone = &channel->tone;
  Tone_FromPeriod(tone, period);

  tone->ticks = 4 * (1 << NOTE_TICKS_PRECISION);
  channel->sample = sample;
  channel->position++;

  return tone;
}

static void
ModuleChannel_TickTone(
    unused void *self,
    SoundChannel *channel,
    const Tone *tone)
{
  unsigned int frequency = Tone_GetModFrequency(tone);
  SoundChannel_Pitch(channel, frequency);
}

static int
ModuleChannel_GetSample(
    void *self,
    unsigned int index)
{
  ModuleChannel *channel = self;

  unsigned int number = channel->sample;
  if (number == 0) {
    return 0;
  }

  ModuleTrack *module = channel->module;
  ModuleSample *sample = &module->samples[number - 1];

  // TODO get rid of modulo operator
  unsigned int position = sample->data + (index % sample->length);

  Reader *reader = module->reader;
  Reader_SeekTo(reader, position);

  char byte = 0;
  Reader_ReadInt8(reader, &byte);

  return byte;
}

static inline void
ModuleTrack_ParseOrders(ModuleTrack *module) {
  const Reader *reader = module->reader;

  Reader_SeekTo(reader, ORDERS_POSITION);
  Reader_ReadUInt8(reader, &module->numOrders);

  Reader_SeekTo(reader, ORDERS_POSITION + 2);
  for (unsigned int i = 0; i < length(module->orders); i++) {
    Reader_ReadUInt8(reader, &module->orders[i]);
    module->numPatterns = Math_max(module->numPatterns, module->orders[i] + 1);
  }
}

static inline void
ModuleTrack_ParseSamples(ModuleTrack *module) {
  const Reader *reader = module->reader;

  unsigned int data = module->numChannels * 4 * 64 * module->numPatterns + PATTERN_POSITION;
  Reader_SeekTo(reader, SAMPLES_POSITION);
  for (unsigned int i = 0; i < length(module->samples); i++) {
    ModuleSample *sample = &module->samples[i];

    // skip name of module sample
    char name[22];
    Reader_ReadValue(reader, name, length(name));

    Reader_ReadUInt16(reader, &sample->length);
    Reader_ReadUInt8(reader, &sample->finetune);
    Reader_ReadUInt8(reader, &sample->volume);
    Reader_ReadUInt16(reader, &sample->loop.start);
    Reader_ReadUInt16(reader, &sample->loop.length);

    sample->length = ModuleTrack_ValueOf(sample->length);
    sample->loop.start = ModuleTrack_ValueOf(sample->loop.start);
    sample->loop.length = ModuleTrack_ValueOf(sample->loop.length);

    sample->data = data;
    data += sample->length;
  }
}

static bool
ModuleTrack_ParseTrack(ModuleTrack *module) {
  const Reader *reader = module->reader;

  int signature;
  if (!Reader_SeekTo(reader, SIGNATURE_POSITION) || !Reader_ReadInt32(reader, &signature)) {
    return false;
  }

  switch (signature) {
  case ModuleTrack_SignatureValue('M', '.', 'K', '.'):
    module->numChannels = 4;
    break;
  case ModuleTrack_SignatureValue('6', 'C', 'H', 'N'):
    module->numChannels = 6;
    break;
  case ModuleTrack_SignatureValue('8', 'C', 'H', 'N'):
    module->numChannels = 8;
    break;
  default:
    return false;
  }

  ModuleTrack_ParseOrders(module);
  ModuleTrack_ParseSamples(module);
  // patterns are parsed on the fly (when a new note/tone is requested)

  return true;
}

bool
ModuleTrack_FromReader(
    ModuleTrack *module,
    Reader *reader)
{
  module->reader = reader;
  module->numChannels = 0;
  module->numPatterns = 0;
  module->numOrders = 0;

  for (unsigned int i = 0; i < length(module->channels); i++) {
    ModuleChannel *channel = &module->channels[i];
    channel->module = module;
    channel->track.self = channel;
    channel->track.Next = ModuleChannel_NextTone;
    channel->sampler.self = channel;
    channel->sampler.Tick = ModuleChannel_TickTone;
    channel->sampler.Get = ModuleChannel_GetSample;
    channel->number = i;
    channel->position = 0;
  }

  if (!ModuleTrack_ParseTrack(module)) {
    return false;
  }

  return true;
}

SoundTrack*
ModuleTrack_GetSoundTrack(
    ModuleTrack *track,
    unsigned int channel)
{
  if (channel > track->numChannels || channel > length(track->channels)) {
    return NULL;
  }

  return &track->channels[channel].track;
}

SoundSampler*
ModuleTrack_GetSoundSampler(
    ModuleTrack *track,
    unsigned int channel)
{
  if (channel > track->numChannels || channel > length(track->channels)) {
    return NULL;
  }

  return &track->channels[channel].sampler;
}
