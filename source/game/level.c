
#include <game/level.h>

int
Level_Convert(
    Level *from,
    Level *to)
{
  char name[15];
  Level_GetName(from, name);
  Level_SetName(to, name);

  int id = Level_GetId(from);
  Level_SetId(to, id);

  int index = 0;
  while (true) {
    Chunk chunk = {0};
    Chunk_AssignIndex(&chunk, index++);

    if (Level_GetChunk(from, &chunk)) {
      if (!Level_AddChunk(to, &chunk)) {
        return -1;
      }
    }
    else {
      return index - 1;
    }
  }
}
