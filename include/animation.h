#ifndef ANIMATION_H
#define ANIMATION_H

#include <gba.h>

typedef short (*Timing)(short dtime);

typedef struct Animation {
  Timing timing;
  bool running;
  short time;
  int from, to;
  int value;
} Animation;

#define Animation_From(from, to, timing) (Animation) { timing, false, 0, from, to, from }

short
Timing_EaseOut(short time);

static inline int
Animation_CurrentValue(Animation *animation) {
  return animation->value;
}

static inline bool
Animation_IsRunning(Animation *animation) {
  return animation->running;
}

static inline void
Animation_Start(Animation *animation) {
  if (!animation->running) {
    animation->running = true;
    animation->time = 0;
  }
}

bool
Animation_Tick(
    Animation *animation,
    short dtime);

static inline void
Animation_Restart(Animation *animation) {
  animation->time = 0;
  animation->value = animation->from;
  animation->running = true;
}

static inline void
Animation_Cancel(Animation *animation) {
  animation->time = 0;
  animation->value = animation->to;
  animation->running = false;
}

#endif
