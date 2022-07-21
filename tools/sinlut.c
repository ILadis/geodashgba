
#include <stdio.h>
#include <math.h>

// see: https://stackoverflow.com/questions/10067510/fixed-point-arithmetic-in-c-programming
int main(int argc, char **argv) {
  const double pi = 3.14159265358979323846;

  int size = 256;

  double step = 2*pi / size;
  double alpha = 0.0;

  FILE *fp = stdout;

  fprintf(fp, "const signed short sinlut[%d] = {\n", size);

  while (size-- > 0) {
    double number = sin(alpha);
    int fixed = (int) round(number * 256) & 0xFFFF;

    alpha += step;

    fprintf(fp, "0x%04X", fixed);
    fputs(",", fp);

    if (size % 8 == 0) fputs("\n", fp);
    if (size % 64 == 0 && size > 0) fputs("\n", fp);
  }

  fputs("};", fp);
  fclose(fp);
}
