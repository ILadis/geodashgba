
#include <animation.h>

#define FRAMES 256

short
Timing_EaseOut(short time) {
  extern const signed short bezlut[FRAMES];
  return bezlut[time % FRAMES];
}

bool
Animation_Tick(
    Animation *animation,
    short dtime)
{
  bool running = animation->running;
  if (!running) {
    return false;
  }

  int value = animation->from;
  int delta = animation->delta;

  short time = animation->time + dtime;

  if (time >= FRAMES) {
    time = 0;
    running = false;
    value = animation->to;
  }
  else {
    value += (animation->timing(time) * delta) >> 8;
  }

  animation->time = time;
  animation->value = value;
  animation->running = running;

  return running;
}
