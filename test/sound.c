
#include <sound.h>
#include "test.h"

test(NextTone_ShouldReturnToneWithExpectedNoteAndLength) {
  // arrange
  const char notes[] = "D`^2.";
  SoundTrack *track = AsciiSoundTrack_FromNotes(&(AsciiSoundTrack){0}, notes);

  // act
  const Tone *tone = SoundTrack_NextTone(track);

  // assert
  assert(tone->note == NOTE_DSHARP);
  assert(tone->octave == 5);
  assert(tone->ticks == 3 * (1 << NOTE_TICKS_PRECISION));
}

test(Fill_ShouldReturnExpectedAmountOfSamples) {
  // arrange
  const char notes[] = "C D/2";
  int buffer[8192] = {0};

  const SoundSampler *sampler = SineSoundSampler_GetInstance();
  SoundTrack *track= AsciiSoundTrack_FromNotes(&(AsciiSoundTrack){0}, notes);

  SoundChannel channel = {0};
  SoundChannel_SetTrackAndSampler(&channel, track, sampler);

  SoundPlayer player = {0};
  SoundPlayer_SetFrequency(&player, 8192);
  SoundPlayer_AddChannel(&player, &channel);

  SoundChannel_SetTempo(&channel, 8192);

  // act
  int length1 = SoundChannel_Fill(&channel, buffer, length(buffer));
  int length2 = SoundChannel_Fill(&channel, buffer, length(buffer));
  int length3 = SoundChannel_Fill(&channel, buffer, length(buffer));
  int length4 = SoundChannel_Fill(&channel, buffer, length(buffer));

  // assert
  assert(length1 == 8192);
  assert(length2 == 4096);
  assert(length3 == 0 && length4 == 0);
}

test(MixChannels_ShouldReturnFalseWhenTrackIsDonePlayingAndResetActiveBuffer) {
  // arrange
  const char notes[] = "C D/2";

  const SoundSampler *sampler = SineSoundSampler_GetInstance();
  SoundTrack *track = AsciiSoundTrack_FromNotes(&(AsciiSoundTrack){0}, notes);

  SoundChannel channel = {0};
  SoundChannel_SetTrackAndSampler(&channel, track, sampler);

  SoundPlayer *player = SoundPlayer_GetInstance();
  SoundPlayer_SetFrequency(player, 8192);
  SoundPlayer_AddChannel(player, &channel);

  SoundChannel_SetTempo(&channel, 8192);

  // act
  int mixes = 0;
  while (SoundPlayer_MixChannels(player)) mixes++;
  bool result = SoundPlayer_MixChannels(player);

  // assert
  assert(result == false);
  assert(mixes == (8192 + 4096) / player->size);
  for (unsigned int i = 0; i < player->size; i++) {
    assert(player->active[i] == 0x00);
  }
}

suite(
  NextTone_ShouldReturnToneWithExpectedNoteAndLength,
  Fill_ShouldReturnExpectedAmountOfSamples,
  MixChannels_ShouldReturnFalseWhenTrackIsDonePlayingAndResetActiveBuffer);
