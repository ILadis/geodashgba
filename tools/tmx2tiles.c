
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int nexttok(FILE *fp, char *buffer, char *delims);

int main() {
  FILE *in = stdin, *out = stdout;

  char buffer[512];
  char *delims = " ,\"=<>\t\r\n";

  fprintf(out, "#include <gba.h>\n");
  fprintf(out, "\n");

root:
  while (nexttok(in, buffer, delims)) {
    if (strcmp(buffer, "layer") == 0) {
      goto layer;
    }
  }

layer:
  while (nexttok(in, buffer, delims)) {
    if (strcmp(buffer, "name") == 0) {
      nexttok(in, buffer, delims);
      fprintf(out, "\n");
      fprintf(out, "const GBA_TileMapRef %sTileMap = (GBA_TileMapRef) {\n", buffer);
    }

    if (strcmp(buffer, "width") == 0) {
      nexttok(in, buffer, delims);
      fprintf(out, "  .width = %s,\n", buffer);
    }

    if (strcmp(buffer, "height") == 0) {
      nexttok(in, buffer, delims);
      fprintf(out, "  .height = %s,\n", buffer);
    }

    if (strcmp(buffer, "data") == 0) {
      fprintf(out, "  .tiles = GBA_Tiles_Of(\n");
      goto data;
    }

    if (strcmp(buffer, "/layer") == 0) {
      fprintf(out, "};\n");
      goto root;
    }
  }

data:
  while (nexttok(in, buffer, delims)) {
    long tile = atol(buffer);
    if (tile != 0 || buffer[0] == '0') {
      // works only when first gid is 1
      // see: https://doc.mapeditor.org/en/stable/reference/global-tile-ids/#gid-tile-flipping
      int tileId = (tile & 0x0FFFFFF) - 1;
      int vFlip = (tile >> 30) & 1;
      int hFlip = (tile >> 31) & 1;
      fprintf(out, "    { .tileId = %d, .vFlip = %d, .hFlip = %d },\n", tileId, vFlip, hFlip);
    }

    if (strcmp(buffer, "/data") == 0) {
      fprintf(out, "  ),\n");
      goto layer;
    }
  }
}

int nexttok(FILE *fp, char *buffer, char *delims) {
  int length = 0;

  do {
    int byte = fgetc(fp);
    if (byte == -1) {
      return length;
    }

    char *delim = delims;
    do {
      if (*delim == byte) {
        if (length == 0) {
          return nexttok(fp, buffer, delims);
        }

        buffer[length] = '\0';
        return length;
      }

      delim++;
    } while (*delim != '\0');

    buffer[length++] = (char) (byte & 0xFF);
  } while (true);
}
