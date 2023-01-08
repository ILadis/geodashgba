
#include <game/level.h>
#include <stdio.h>

int main(int argc, char **argv) {
  unsigned char buffer[20480] = {0};

  LevelId id = 0;
  FILE *fp = stdout, *log = stderr;

  while (true) {
    Level out = Level_FromBuffer(buffer);
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
        fprintf(log, "Level #%ld, wrote %ld chunk(s)\n", id, --index);
        break;
      }
    }

    fprintf(fp, "#include <game/level.h>\n\n");
    fprintf(fp, "static const unsigned char data%02ld[] = {", id);

    int length = out.cursor.x;
    for (int i = 0; i < length; i++) {
      if (i % 8 == 0) fputs("\n", fp);

      fprintf(fp, "0x%02x", buffer[i]);
      fprintf(fp, ",");
    }

    fprintf(fp, "\n};\n\n");
    fprintf(fp, "Level course%02ld = Level_FromBuffer(data%02ld);\n", id, id);
  }
  fclose(fp);

  return 0;
}
