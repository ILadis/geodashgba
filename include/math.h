#ifndef MATH_H
#define MATH_H

int
Math_abs(int num);

int
Math_min(int num1, int num2);

int
Math_max(int num1, int num2);

int
Math_signum(int num);

int
Math_clamp(int num, int min, int max);

int
Math_round(int num, int decimals);

static inline int
Math_floor(int num, int decimals) {
  return (num >> decimals);
}

int
Math_log2(int num);

int
Math_mod2(int num, int mod);

int
Math_sin(int alpha);

int
Math_cos(int alpha);

int
Math_rand();

int
Math_div(int num, int denom);

int
Math_atan2(int x, int y);

#endif
