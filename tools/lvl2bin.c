
#include <game/level.h>
#include <stdio.h>

int main(int argc, char **argv) {
  LevelId id = 0;
  FILE *fp = stdout, *log = stderr;

  fprintf(fp, "\n");
  fprintf(fp, "#include <game/level.h>\n");

  while (true) {
    Level out = Level_AllocateNew(30720);
    Level *in = Level_GetById(id++);

    if (in == NULL) break;

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
        fprintf(log, "Level #%d, wrote %d chunk(s)\n", id, --index);
        break;
      }
    }

    fprintf(fp, "\n");
    fprintf(fp, "Level course%02d = Level_FromData(", id);

    int length = out.cursor.x;
    const unsigned char *buffer = out.buffer.read;
    for (int i = 0; i < length; i++) {
      fprintf(fp, i % 8 == 0 ? "\n  " : " ");
      fprintf(fp, "0x%02x", buffer[i]);
      fprintf(fp, ",");
    }

    fprintf(fp, "\n);\n");
  }
  fclose(fp);

  return 0;
}
