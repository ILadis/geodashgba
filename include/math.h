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
Math_sin(int alpha);

int
Math_cos(int alpha);

int
Math_div(int num, int denom);

short
Math_atan2(short x, short y);

#endif
