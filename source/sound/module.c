
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

/* Note: these are the amiga period values for the first octave (C-1 to B-1)
 * for each of the 16 available finetune levels. Module samples specify which
 * finetune level should be used when playing back sound.
 */
static const unsigned int FinetunePeriod1[][12] = {
  // Finetune 0
  { 856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453 },
  // Finetune 1
  { 850, 802, 757, 715, 674, 637, 601, 567, 535, 505, 477, 450 },
  // Finetune 2
  { 844, 796, 752, 709, 670, 632, 597, 563, 532, 502, 474, 447 },
  // Finetune 3
  { 838, 791, 746, 704, 665, 628, 592, 559, 528, 498, 470, 444 },
  // Finetune 4
  { 832, 785, 741, 699, 660, 623, 588, 555, 524, 495, 467, 441 },
  // Finetune 5
  { 826, 779, 736, 694, 655, 619, 584, 551, 520, 491, 463, 437 },
  // Finetune 6
  { 820, 774, 730, 689, 651, 614, 580, 547, 516, 487, 460, 434 },
  // Finetune 7
  { 814, 768, 725, 684, 646, 610, 575, 543, 513, 484, 457, 431 },
  // Finetune -8
  { 907, 856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480 },
  // Finetune -7
  { 900, 850, 802, 757, 715, 675, 636, 601, 567, 535, 505, 477 },
  // Finetune -6
  { 894, 844, 796, 752, 709, 670, 632, 597, 563, 532, 502, 474 },
  // Finetune -5
  { 887, 838, 791, 746, 704, 665, 628, 592, 559, 528, 498, 470 },
  // Finetune -4
  { 881, 832, 785, 741, 699, 660, 623, 588, 555, 524, 494, 467 },
  // Finetune -3
  { 875, 826, 779, 736, 694, 655, 619, 584, 551, 520, 491, 463 },
  // Finetune -2
  { 868, 820, 774, 730, 689, 651, 614, 580, 547, 516, 487, 460 },
  // Finetune -1
  { 862, 814, 768, 725, 684, 646, 610, 575, 543, 513, 484, 457 },
};

static inline unsigned int
Tone_GetFinetunePeriod(const Tone *tone, unsigned char finetune) {
  return (tone->note > length(FinetunePeriod1[finetune])) ? 0 : (FinetunePeriod1[finetune][tone->note] * 2) >> tone->octave;
}

/* Note: this converts amiga periods to samples/s (Hz). This value determines
 * the amount of samples that should be played per second for the given note.
 */
static inline unsigned int
Tone_GetFrequency(const Tone *tone, unsigned char finetune) {
  return Math_div(3579545, Tone_GetFinetunePeriod(tone, finetune));
}

// finds the closest note for the given amiga period value
static inline void
Tone_FromPeriod(
    Tone *tone,
    unsigned int period)
{
  unsigned int closest = ~0;

  if (period == 0) {
    tone->octave = 1;
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

static bool
ModuleSoundTrack_VerifySignature(const Reader *reader) {
  int signature;
  if (!Reader_SeekTo(reader, SIGNATURE_POSITION) || !Reader_ReadInt32(reader, &signature)) {
    return false;
  }

  switch (signature) {
  case ModuleTrack_SignatureValue('M', '.', 'K', '.'):
  case ModuleTrack_SignatureValue('6', 'C', 'H', 'N'):
  case ModuleTrack_SignatureValue('8', 'C', 'H', 'N'):
    return true;
  default:
    return false;
  }
}

static bool
ModuleSoundTrack_NumberOfChannels(
    const Reader *reader,
    unsigned int *channels)
{
  int signature;
  if (!Reader_SeekTo(reader, SIGNATURE_POSITION) || !Reader_ReadInt32(reader, &signature)) {
    return false;
  }

  switch (signature) {
  case ModuleTrack_SignatureValue('M', '.', 'K', '.'):
    *channels = 4;
    break;
  case ModuleTrack_SignatureValue('6', 'C', 'H', 'N'):
    *channels = 6;
    break;
  case ModuleTrack_SignatureValue('8', 'C', 'H', 'N'):
    *channels = 8;
    break;
  default:
    return false;
  }

  return true;
}

static inline bool
ModuleSoundTrack_NumberOfOrders(
    const Reader *reader,
    unsigned char *orders)
{
  if (!Reader_SeekTo(reader, ORDERS_POSITION)) {
    return false;
  }

  return Reader_ReadUInt8(reader, orders);
}

static inline bool
ModuleSoundTrack_NumberOfPatterns(
    const Reader *reader,
    unsigned char *patterns)
{
  if (!Reader_SeekTo(reader, ORDERS_POSITION + 2)) {
    return false;
  }

  unsigned char count = 0;
  for (unsigned int i = 0; i < 32; i++) {
    unsigned char pattern = 0;
    if (!Reader_ReadUInt8(reader, &pattern)) {
      return false;
    }

    count = Math_max(count, pattern + 1);
  }

  *patterns = count;
  return true;
}

static inline bool
ModuleSoundTrack_FinetuneOfSample(
    const Reader *reader,
    unsigned int sample,
    unsigned char *finetune)
{
  unsigned int position = SAMPLES_POSITION + (30 * sample) + 24;
  if (!Reader_SeekTo(reader, position)) {
    return false;
  }

  return Reader_ReadUInt8(reader, finetune);
}

static inline bool
ModuleSoundTrack_VolumeOfSample(
    const Reader *reader,
    unsigned int sample,
    unsigned char *volume)
{
  unsigned int position = SAMPLES_POSITION + (30 * sample) + 25;
  if (!Reader_SeekTo(reader, position)) {
    return false;
  }

  return Reader_ReadUInt8(reader, volume);
}

static inline bool
ModuleSoundTrack_LengthOfSample(
    const Reader *reader,
    unsigned int sample,
    unsigned int *length)
{
  unsigned int position = SAMPLES_POSITION + (30 * sample) + 22;
  if (!Reader_SeekTo(reader, position)) {
    return false;
  }

  unsigned short value = 0;
  if (!Reader_ReadUInt16(reader, &value)) {
    return false;
  }

  *length = ModuleTrack_ValueOf(value);
  return true;
}

static inline bool
ModuleSoundTrack_PositionOfSample(
    const Reader *reader,
    unsigned int sample,
    unsigned int *position)
{
  unsigned int lengths = 0;

  for (unsigned int i = 0; i < sample; i++) {
    unsigned int length = 0;
    if (!ModuleSoundTrack_LengthOfSample(reader, i, &length)) {
      return false;
    }

    lengths += length;
  }

  unsigned int channels = 0;
  if (!ModuleSoundTrack_NumberOfChannels(reader, &channels)) {
    return false;
  }

  unsigned char patterns = 0;
  if (!ModuleSoundTrack_NumberOfPatterns(reader, &patterns)) {
    return false;
  }

  *position = PATTERN_POSITION + (channels * 4 * 64 * patterns) + lengths;
  return true;
}

static bool
ModuleSoundTrack_CurrentOrder(
    ModuleSoundTrack *track,
    unsigned char *order)
{
  const Reader *reader = track->reader;

  // divide position by 64 to get current order index
  unsigned int index = track->position >> 6;
  unsigned int position = ORDERS_POSITION + 2 + index;

  if (!Reader_SeekTo(reader, position)) {
    return false;
  }

  return Reader_ReadUInt8(reader, order);
}

static bool
ModuleSoundTrack_CurrentPattern(
    ModuleSoundTrack *track,
    unsigned char pattern[4])
{
  const Reader *reader = track->reader;

  unsigned char order = 0;
  if (!ModuleSoundTrack_CurrentOrder(track, &order)) {
    return false;
  }

  unsigned int channels = 0;
  if (!ModuleSoundTrack_NumberOfChannels(reader, &channels)) {
    return NULL;
  }

  // current row is calculated by modulo 64
  unsigned int row = track->position & 63;

  // 64 patterns per order and 4 bytes per pattern for each channel
  unsigned int offset = PATTERN_POSITION
    + (channels * 64 * 4 * order)
    + (channels * 4 * row)
    + (track->channel * 4);

  if (!Reader_SeekTo(reader, offset)) {
    return NULL;
  }

  return Reader_Read(reader, pattern, 4);
}

static inline bool
ModuleSoundTrack_HasNextTone(ModuleSoundTrack *track) {
  const Reader *reader = track->reader;

  unsigned char orders = 0;
  if (!ModuleSoundTrack_NumberOfOrders(reader, &orders)) {
    return false;
  }

  /* Note: multiply length of module track (= number of orders) by number of patterns
   * per order (= 64) to get the number of total patterns in this track.
   */
  unsigned int length = orders * 64;
  return track->position < length;
}

static const Tone*
ModuleSoundTrack_NextTone(void *self) {
  ModuleSoundTrack *track = self;

  if (!ModuleSoundTrack_HasNextTone(track)) {
    return NULL;
  }

  unsigned char pattern[4];
  if (!ModuleSoundTrack_CurrentPattern(track, pattern)) {
    return NULL;
  }

  unsigned int sample =  (pattern[0] & 0b11110000) + (pattern[2] >> 4);
  unsigned int period = ((pattern[0] & 0b00001111) << 8) + pattern[1];
  unsigned char effect = (pattern[2] & 0b00001111);
  unsigned char param  =  pattern[3];

  // FIXME reconsider if periods should be used instead of frequencies
  switch (effect) {
  case SOUND_EFFECT_PORTA_UP:
  case SOUND_EFFECT_PORTA_DOWN:
    // convert param from period to frequency (Hz)
    param = param != 0 ? Math_div(3579545, param) : param;
    break;
  }

  Tone *tone = &track->tone;
  Tone_FromPeriod(tone, period);

  /* Note: sample number 0 means the current played sample should not be
   * changed. The sound channel is also using 0-indexed sampler numbers,
   * which means the first sample has number 0. To convert the sample
   * to 0-indexed numbers 1 is subtracted.
   */

  tone->sample = sample - 1;
  tone->effect.type = effect;
  tone->effect.param = param;

  track->position++;

  return tone;
}

static bool
ModuleSoundTrack_SeekTo(
    void *self,
    unsigned int position)
{
  ModuleSoundTrack *track = self;
  unsigned int previous = track->position;

  track->position = position;

  if (!ModuleSoundTrack_HasNextTone(track)) {
    track->position = previous; // reset to old position if seeking fails
    return false;
  }

  return true;
}

SoundTrack*
ModuleSoundTrack_From(
    ModuleSoundTrack *track,
    const Reader *reader,
    unsigned int channel)
{
  track->reader = reader;
  track->channel = channel;
  track->position = 0;

  unsigned int channels = 0;
  if (!ModuleSoundTrack_NumberOfChannels(reader, &channels) || channel >= channels) {
    return NULL;
  }

  track->base.self = track;
  track->base.Next = ModuleSoundTrack_NextTone;
  track->base.SeekTo = ModuleSoundTrack_SeekTo;

  return &track->base;
}

static unsigned int
ModuleSoundSampler_GetLength(void *self) {
  ModuleSoundSampler *sampler = self;
  const Reader *reader = sampler->reader;

  unsigned int length = 0;
  if (!ModuleSoundTrack_LengthOfSample(reader, sampler->index, &length)) {
    return 0;
  }

  return length;
}

static unsigned int
ModuleSoundSampler_GetFrequency(
    void *self,
    const Tone *tone)
{
  ModuleSoundSampler *sampler = self;
  const Reader *reader = sampler->reader;

  unsigned char finetune = 0;
  if (!ModuleSoundTrack_FinetuneOfSample(reader, sampler->index, &finetune)) {
    return 0;
  }

  return Tone_GetFrequency(tone, finetune);
}

static unsigned char
ModuleSoundSampler_GetVolume(void *self) {
  ModuleSoundSampler *sampler = self;
  const Reader *reader = sampler->reader;

  unsigned char volume = 0;
  if (!ModuleSoundTrack_VolumeOfSample(reader, sampler->index, &volume)) {
    return 0;
  }

  return volume;
}

static inline int
ModuleSoundSampler_GetSample(
    void *self,
    unsigned int index)
{
  ModuleSoundSampler *sampler = self;
  const Reader *reader = sampler->reader;

  unsigned int position = 0;
  if (!ModuleSoundTrack_PositionOfSample(reader, sampler->index, &position)) {
    return 0;
  }

  // TODO properly apply loop start/length and volume
  unsigned int length = 0;
  if (!ModuleSoundTrack_LengthOfSample(reader, sampler->index, &length) || length == 0) {
    return 0;
  }

  position += index % length;
  if (!Reader_SeekTo(reader, position)) {
    return 0;
  }

  char value = 0;
  Reader_ReadInt8(reader, &value);

  return value;
}

static int*
ModuleSoundSampler_FillBuffer(
    void *self, int *buffer,
    unsigned int *position,
    unsigned int increment,
    unsigned char volume,
    unsigned int size)
{
  ModuleSoundSampler *sampler = self;
  return SoundSampler_SlowFillBuffer(&sampler->base, buffer, position, increment, volume, size);
}

SoundSampler*
ModuleSoundSampler_From(
    ModuleSoundSampler *sampler,
    const Reader *reader,
    unsigned int index)
{
  sampler->reader = reader;
  sampler->index = index;

  if (!ModuleSoundTrack_VerifySignature(reader) || index >= 32) {
    return NULL;
  }

  sampler->base.self = sampler;
  sampler->base.Get  = ModuleSoundSampler_GetSample;
  sampler->base.Fill = ModuleSoundSampler_FillBuffer;
  sampler->base.Length = ModuleSoundSampler_GetLength;
  sampler->base.Frequency = ModuleSoundSampler_GetFrequency;
  sampler->base.Volume = ModuleSoundSampler_GetVolume;

  return &sampler->base;
}
