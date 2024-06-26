
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

void header(FILE *fp, const char *format, ...);
void pixel(FILE *fp, float depth, unsigned int *color);

// https://en.wikipedia.org/wiki/Netpbm
int main(int argc, char **argv) {
  FILE *in = stdin, *out = stdout, *log = stderr;

  if (argc != 3) {
    fprintf(log, "Invalid number of arguments given.\n");
    return 1;
  }

  struct {
    const char *name;
    int width, height;
    int size;
  } font = {0};

  font.name = argv[2];
  sscanf(argv[1], "%dx%d", &font.width, &font.height);

  // width is optional for a variable width font
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

  while (true) {
    long offset = ftell(in);
    int width = font.width;

    // look ahead and determine width of next glyph
    for (int y = 0; y < font.height; y++) {
      for (int x = 0; x < image.width; x++) {
        unsigned int color;
        pixel(in, image.depth, &color);

        if (color != 0xFFFFFF && x >= width) {
          width = x + 1;
        }
      }
    }

    // stop if look ahead did not complete
    if (feof(in)) break;

    struct {
      const char *name;
      const unsigned int color;
    } fields[] = {
      { "fill",      0x0000FF },
      { "outline",   0xFF0000 },
      { "highlight", 0x00FF00 },
    };

    fprintf(out, "\n");
    fprintf(out, "static const Glyph glyph%d = {\n", font.size++);

    for (int j = 0; j < 3; j++) {
      fseek(in, offset, SEEK_SET);
      fprintf(out, "  .%s = (const unsigned char[]) {\n", fields[j].name);

      for (int y = 0; y < font.height; y++) {
        char byte[] = "00000000";

        for (int x = 0; x < image.width; x++) {
          int index = x % 8;

          unsigned int color;
          pixel(in, image.depth, &color);

          if (index == 0 && x <= (width | 7)) {
            fprintf(out, x == 0 ? "    " : ", ");
          }

          char *bit = &byte[7 - index];
          if (color == fields[j].color) {
            *bit = '1';
          } else {
            *bit = '0';
          }

          if (index == 7 && x <= (width | 7)) {
            fprintf(out, "0b%s", byte);
          }
        }

        fprintf(out, width == 0 ? "    0b%s,\n" : ",\n", byte);
      }

      fprintf(out, "  },\n");
    }

    fprintf(out, "  .width = %d,\n", width);
    fprintf(out, "};\n");
  }

  fprintf(out, "\n");
  fprintf(out, "const Font %sFont = {\n", font.name);
  fprintf(out, "  .height = %d,\n", font.height);
  fprintf(out, "  .glyphs = {\n");

  for (int i = 0; i < font.size; i++) {
    fprintf(out, "    [%d] = &glyph%d,\n", i, i);
  }

  fprintf(out, "  },\n");
  fprintf(out, "};\n");

  fclose(in);
  fclose(out);
}

void header(FILE *fp, const char *format, ...) {
  char line[256] = {0};

  do {
    fgets(line, sizeof(line), fp);
  } while (line[0] == '#');

  va_list arguments;
  va_start(arguments, format);

  vsscanf(line, format, arguments);
  va_end(arguments);
}

void pixel(FILE *fp, float depth, unsigned int *pixel) {
  int red   = fgetc(fp);
  int green = fgetc(fp);
  int blue  = fgetc(fp);

  union {
    unsigned int value;
    struct {
      unsigned int blue: 8;
      unsigned int green: 8;
      unsigned int red: 8;
    };
  } color = {0};

  color.red = (red / depth) * 0xFF;
  color.green = (green / depth) * 0xFF;
  color.blue = (blue / depth) * 0xFF;

  *pixel = color.value;
}
