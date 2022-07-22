
#include <math.h>

extern const signed short sinlut[256];

int
Math_abs(int num) {
  return num < 0 ? -num : num;
}

int
Math_min(int num1, int num2) {
  return num1 < num2 ? num1 : num2;
}

int
Math_max(int num1, int num2) {
  return num1 > num2 ? num1 : num2;
}

int
Math_signum(int num) {
  return num < 0 ? -1 : num > 0 ? +1 : 0;
}

int
Math_clamp(int num, int min, int max) {
  if (num < min) return min;
  else if (num > max) return max;
  else return num;
}

int
Math_sin(int alpha) {
  return sinlut[alpha & 0xFF];
}

int
Math_cos(int alpha) {
  return sinlut[(alpha + 64) & 0xFF];
}

#ifdef NOGBA

int
Math_div(int num, int denom) {
  return num / denom;
}

short
Math_atan2(short x, short y) {
  return 0;
}

#else

int
Math_div(int num, int denom) {
  int result;
  asm volatile("swi 0x06" : "=r" (result));
  return result;
}

short
Math_atan2(short x, short y) {
  short result;
  asm volatile("swi 0x0a" : "=r" (result));
  return (result >> 8) - 64;
}

#endif
