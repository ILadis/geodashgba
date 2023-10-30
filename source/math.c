
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
Math_round(int num, int decimals) {
  int round = (1 << (decimals - 1)) + 1;
  return (num + round) >> decimals;
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
  const double size = 256;

  double rads = 2*pi * (alpha / size);

  double number = sin(rads);
  int fixed = (int) round(number * size);

  return fixed;
}

int
Math_div(int num, int denom) {
  return num / denom;
}

int
Math_atan2(int x, int y) {
  extern double atan2(double y, double x);
  extern double round(double arg);

  const double pi = 3.14159265358979323846;
  const double size = 256;

  double rads = size / (2*pi);

  double number = atan2(y, x);
  if (number < 0) number += pi;

  int fixed = (int) round(number * rads);

  return fixed;
}

#else

int
Math_sin(int alpha) {
  // returns signed 1.8 fixed-point integer
  extern const signed short sinlut[256];
  return sinlut[alpha & 0xFF];
}

int
Math_div(int num, int denom) {
  register int r0 asm("r0") = num;
  register int r1 asm("r1") = denom;
  asm volatile("swi 0x06" : "=r"(r0) : "r"(r0), "r"(r1));
  return r0;
}

int
Math_atan2(int x, int y) {
  // returns unsigned 1.16 fixed-point integer
  register int r0 asm("r0") = x;
  register int r1 asm("r1") = y;
  asm volatile("swi 0x0a" : "=r"(r0) : "r"(r0), "r"(r1));
  return r0;
}

#endif
