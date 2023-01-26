
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef union Color {
  int value;
  struct {
    int blue: 8;
    int green: 8;
    int red: 8;
  };
} Color;

void header(FILE *fp, const char *format, ...);
void pixel(FILE *fp, int depth, Color *color);

// https://en.wikipedia.org/wiki/Netpbm
int main(int argc, char **argv) {
  FILE *in = stdin, *out = stdout, *log = stderr;

  if (argc != 3) {
    fprintf(log, "Invalid number of arguments given.\n");
    return 1;
  }

  struct {
    int width, height;
    int size;
    const char *name;
  } font = {0};

  font.name = argv[2];
  sscanf(argv[1], "%dx%d", &font.width, &font.height);

  // glyph width is optional for a variable width font
  if (font.height <= 0) {
    fprintf(log, "Invalid font height given.\n");
    return 1;
  }

  char format;
  header(in, "P%c", &format);

  if (format != '6') {
    fprintf(log, "Invalid file format, only P6 is supported.\n");
    return 1;
  }

  struct {
    int width, height;
    int depth;
  } image = {0};

  header(in, "%d %d", &image.width, &image.height);
  if (image.width <= 0 || image.height <= 0) {
    fprintf(log, "Invalid image dimensions (width or height).\n");
    return 1;
  }

  header(in, "%d", &image.depth);
  if (image.depth <= 0 || image.depth >= 256) {
    fprintf(log, "Invalid image bit depth.\n");
    return 1;
  }

  fprintf(out, "\n");
  fprintf(out, "#include <text.h>\n");

  int msb = 0;
  for (int y = 0; y < image.height;) {
    char byte[] = "00000000";

    if (y % font.height == 0) {
      fprintf(out, "\n");
      fprintf(out, "static const Glyph glyph%ld = {\n", font.size);
      fprintf(out, "  .data = (unsigned char[]) {\n");

      msb = font.width - 2; font.size++;
    }

    for (int x = 0; x < image.width; x++) {
      int i = x % 8;

      Color color;
      pixel(in, image.depth, &color);

      if (i == 0) {
        fprintf(out, x == 0 ? "    " : ", ");
        strcpy(byte, "00000000");
      }

      char *bit = &byte[7 - i];
      if (color.value != 0xFFFFFF) {
        if (x > msb) msb = x;
        *bit = '1';
      }

      if (i == 7) {
        fprintf(out, "0b%s", byte);
      }
    }

    fprintf(out, image.width % 8 == 0 ? ",\n" : "0b%s,\n", byte);

    if (++y % font.height == 0) {
      fprintf(out, "  },\n");
      fprintf(out, "  .width = %ld,\n", msb + 2);
      fprintf(out, "};\n");
    }
  }

  fprintf(out, "\n");
  fprintf(out, "const Font %sFont = {\n", font.name);
  fprintf(out, "  .height = %ld,\n", font.height);
  fprintf(out, "  .glyphs = {\n");

  for (int i = 0; i < font.size; i++) {
      fprintf(out, "    [%ld] = &glyph%ld,\n", i, i);
  }

  fprintf(out, "  },\n");
  fprintf(out, "};\n");

  fclose(in);
  fclose(out);
}

void header(FILE *fp, const char *format, ...) {
  char line[256] = {0};

  do {
    fgets(line, 255, fp);
  } while (line[0] == '#');

  va_list arguments;
  va_start(arguments, format);

  vsscanf(line, format, arguments);
  va_end(arguments);
}

void pixel(FILE *fp, int depth, Color *color) {
  int red   = fgetc(fp);
  int green = fgetc(fp);
  int blue  = fgetc(fp);

  color->red = red;
  color->green = green;
  color->blue = blue;
}
