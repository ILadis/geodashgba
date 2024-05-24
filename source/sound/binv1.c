
#include <sound.h>

static bool
Binv1SoundTrack_AddTone(
    void *self,
    const Tone *tone)
{
  Binv1SoundTrack *track = self;

  Writer *writer = DataSource_AsWriter(track->source);
  Writer_WriteUInt8(writer, (unsigned char) tone->note);
  Writer_WriteUInt8(writer, (unsigned char) tone->effect.type);
  Writer_WriteUInt8(writer, (unsigned char) tone->effect.param);
  Writer_WriteUInt8(writer, (unsigned char) tone->octave);
  Writer_WriteUInt8(writer, (unsigned char) tone->sample);

  return true;
}

static const Tone*
Binv1SoundTrack_NextTone(void *self) {
  Binv1SoundTrack *track = self;
  Tone *tone = &track->tone;

  Reader *reader = DataSource_AsReader(track->source);
  Reader_ReadUInt8(reader, (unsigned char *) &tone->effect.type);
  Reader_ReadUInt8(reader, (unsigned char *) &tone->effect.param);
  Reader_ReadUInt8(reader, (unsigned char *) &tone->octave);
  Reader_ReadUInt8(reader, (unsigned char *) &tone->sample);

  return tone;
}

SoundTrack*
Binv1SoundTrack_From(
    Binv1SoundTrack *track,
    DataSource *source)
{
  track->source = source;

  track->base.self = track;
  track->base.Add  = Binv1SoundTrack_AddTone;
  track->base.Next = Binv1SoundTrack_NextTone;

  return &track->base;
}
