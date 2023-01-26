
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double bezier(const double *points, int length, double time);
double find(double point, const double *points, int length);

int main(int argc, char *argv[]) {
  FILE *out = stdout, *log = stderr;

  if (argc < 2 || argc % 2 != 1) {
    fprintf(log, "Invalid number of arguments given.\n");
    return 1;
  }

  int length = (argc - 1) / 2 + 2;
  double points[length * 2];

  double *px = &points[0];
  double *py = &points[length];

  int index = 0;

  px[index] = 0;
  py[index] = 0;

  char **arguments = &argv[1];
  for (index++; index < length-1; index++) {
    px[index] = atof(*arguments++);
    py[index] = atof(*arguments++);
  }

  px[index] = 1;
  py[index] = 1;

  int size = 256;
  fprintf(out, "const signed short bezlut[%d] = {\n", size);

  double x = 0;
  double step = 1.0 / size;

  while (size-- > 0) {
    double time = find(x, px, length);
    double y = bezier(py, length, time);

    int fixed = (int) round(y * 256) & 0xFFFF;

    x += step;

    fprintf(out, "0x%04X", fixed);
    fprintf(out, ",");

    if (size % 8 == 0) fputs("\n", out);
    if (size % 64 == 0 && size > 0) fputs("\n", out);
  }

  fprintf(out, "};");
  fclose(out);

  return 0;
}

double bezier(const double *points, int length, double time) {
  double pp[--length];

  for (int i = 0; i < length; i++) {
    pp[i] = points[i] * (1-time) + points[i+1] * time;
  }

  if (length > 1) {
    return bezier(pp, length, time);
  }

  return pp[0];
}

double find(double point, const double *points, int length) {
  const double epsilon = 0.000001;
  static double times[2] = { 0, 1 };

  double time = (times[0] + times[1]) / 2;

  double p = bezier(points, length, time);
  double delta = times[1] - times[0];

  if (delta < epsilon) {
    times[0] = 0; times[1] = 1;
    return time;
  }
  else {
    times[p > point] = time;
    return find(point, points, length);
  }
}
