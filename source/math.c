
#include <math.h>

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
Math_log2(int num) {
  int value = 0;

  while ((num & 0x1) == 0) {
    num >>= 1;
    value++;
  }

  return value;
}

int
Math_mod2(int num, int mod) {
  return num & ((1 << mod) - 1);
}

int
Math_cos(int alpha) {
  return Math_sin(alpha + 64);
}

int
Math_rand() {
  static int seed = 0;
  return seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
}

#ifdef NOGBA

int
Math_sin(int alpha) {
  extern double sin(double arg);
  extern double round(double arg);

  const double pi = 3.14159265358979323846;
  double size = 256;

  double rads = 2*pi * (alpha / size);

  double number = sin(rads);
  int fixed = (int) round(number * size);

  return fixed;
}

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
Math_sin(int alpha) {
  // signed 8w fixed-point integer
  extern const signed short sinlut[256];
  return sinlut[alpha & 0xFF];
}

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
