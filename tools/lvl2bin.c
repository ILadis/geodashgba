
#include <game/level.h>
#include <stdio.h>

int main(int argc, char **argv) {
  unsigned char buffer[1024] = {0};

  Level *in = Level_GetById(LEVEL_TEST_COURSE);
  Level out = Level_FromBuffer(buffer);

  FILE *fp = stdout;

  for (int i = 0; i < 3; i++) {
    Chunk chunk = {0};
    Chunk_AssignIndex(&chunk, i);

    Level_GetChunk(in, &chunk);
    Level_AddChunk(&out, &chunk);
  }

  int length = out.cursor.x;

  fprintf(fp, "const unsigned int levelLength = %ld;\n", length);
  fprintf(fp, "const unsigned char levelData[] = {");

  for (int i = 0; i < length; i ++) {
    if (i % 8 == 0) fputs("\n", fp);

    fprintf(fp, "0x%02x", buffer[i]);
    fputs(",", fp);
  }

  fputs("\n};", fp);
  fclose(fp);

  return 0;
}
