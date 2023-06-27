
#include <game/level.h>
#include <stdio.h>
#include <ctype.h>

static char buffer[1024 * 1024 * 5] = {0};

int main(int argc, char **argv) {
  FILE *in = stdin, *out = stdout, *log = stderr;

  int x = 0, y = 0;
  for (int i = 0; i < sizeof(buffer);) {
    int byte = fgetc(in);

    if (byte == EOF) {
      break;
    }

    else if (byte == '\n') {
      x = x > 0 ? x : i;
      y++;
    }

    else if (isprint(byte)) {
      buffer[i++] = (char) byte;
    }
  }

  Level binv1 = Level_AllocateNew(30720);
  Level ascii = {
    .format = FORMAT_ASCII,
    .buffer = buffer,
    .size = { x, y }
  };

  char name[15];
  Level_GetName(&ascii, name);
  Level_SetName(&binv1, name);

  int index = 0;
  while (true) {
    Chunk chunk = {0};
    Chunk_AssignIndex(&chunk, index++);

    if (Level_GetChunk(&ascii, &chunk)) {
      if (!Level_AddChunk(&binv1, &chunk)) {
        fprintf(log, "Could not add chunk to level, size exceeded?\n");
        break;
      }
    }
    else {
      fprintf(log, "Wrote %d chunk(s)\n", --index);
      break;
    }
  }

  int length = binv1.cursor.x;
  const unsigned char *buffer = binv1.buffer.read;
  for (int i = 0; i < length; i++) {
    fputc(buffer[i], out);
  }

  fclose(in);
  fclose(out);
}
