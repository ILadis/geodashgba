#ifndef COUNTER_H
#define COUNTER_H

#include <types.h>

typedef char* Counter;

void
Counter_Reset(
    Counter counter,
    int length);

static inline bool
Counter_IsBlank(Counter counter) {
  return counter[0] == ' ';
}

void
Counter_IncrementOne(Counter counter);

#endif
