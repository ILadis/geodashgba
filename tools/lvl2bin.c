
#include <game/level.h>
#include <stdio.h>

int main(int argc, char **argv) {
  unsigned char buffer[4096] = {0};

  Level *in = Level_GetById(LEVEL_TEST_COURSE);
  Level out = Level_FromBuffer(buffer);

  FILE *fp = stdout, *log = stderr;

  int index = 0;
  while (true) {
    Chunk chunk = {0};
    Chunk_AssignIndex(&chunk, index++);

    if (Level_GetChunk(in, &chunk)) {
      if (!Level_AddChunk(&out, &chunk)) {
        fprintf(log, "Could not add chunk to level, size exceeded?\n");
        break;
      }
    }
    else {
      fprintf(log, "Wrote %ld chunk(s)\n", index);
      break;
    }
  }

  int length = out.cursor.x;

  fprintf(fp, "#include <game/level.h>\n\n");
  fprintf(fp, "static const unsigned char data[%ld] = {", length);

  for (int i = 0; i < length; i ++) {
    if (i % 8 == 0) fputs("\n", fp);

    fprintf(fp, "0x%02x", buffer[i]);
    fprintf(fp, ",");
  }

  fprintf(fp, "\n};\n\n");
  fprintf(fp, "Level testCourse = Level_FromBuffer(data);\n");

  fclose(fp);

  return 0;
}
