
#include <stdio.h>
#include <math.h>

// see: https://stackoverflow.com/questions/10067510/fixed-point-arithmetic-in-c-programming
int main() {
  const double pi = 3.14159265358979323846;

  int size = 256;

  double step = 2*pi / size;
  double alpha = 0.0;

  FILE *out = stdout;

  fprintf(out, "const signed short sinlut[%d] = {\n", size);

  while (size-- > 0) {
    double number = sin(alpha);
    int fixed = (int) round(number * 256) & 0xFFFF;

    alpha += step;

    fprintf(out, "0x%04X", fixed);
    fputs(",", out);

    if (size % 8 == 0) fputs("\n", out);
    if (size % 64 == 0 && size > 0) fputs("\n", out);
  }

  fputs("};", out);
  fclose(out);

  return 0;
}
