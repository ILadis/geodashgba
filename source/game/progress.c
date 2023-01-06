
#include <game/progress.h>

Progress*
Progress_GetInstance() {
  static Progress progress = {0};
  return &progress;
}

void
Progress_SetLevel(
    Progress *progress,
    Level *level)
{
  progress->redraw = true;

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0);

  const Bounds *bounds = Chunk_GetBounds(&chunk);
  int width = bounds->size.x * 2;

  int count = Level_GetChunkCount(level);
  int total = width * count;

  progress->total = total;
}

void
Progress_Update(
    Progress *progress,
    Cube *cube)
{
  const Vector *position = Cube_GetPosition(cube);

  int value = Math_div(position->x << 8, progress->total);
  value = Math_min(value, 1 << 8);

  progress->previous = progress->current;
  progress->current = value;
}

void
Progress_Draw(
    Progress *progress,
    Camera *camera)
{
  static const GBA_BackgroundControl layer = {
    .size = 0, // 256x256
    .colorMode = 1,
    .tileSetIndex = 0,
    .tileMapIndex = 20,
    .priority = 1,
  };

  const int tx = 7, ty = 1;
  const int px = 3 + tx * 8, py = 3 + ty * 8;

  const int width = 123;
  const int tid = 48;

  if (progress->redraw) {
    GBA_EnableBackgroundLayer(2, layer);

    GBA_TileMapRef target;
    GBA_TileMapRef_FromBackgroundLayer(&target, 2);

    GBA_Tile tile = { .tileId = tid };
    for (int i = 0; i < 16; i++) {
      GBA_TileMapRef_BlitTile(&target, tx + i, ty, &tile);
      tile.tileId++;
    }
  }

  else {
    GBA_TileMapRef target;
    GBA_TileMapRef_FromBackgroundLayer(&target, 2);

    GBA_System *system = GBA_GetSystem();
    GBA_Bitmap8 *bitmap = &system->tileSets8[0][tid];

    int previous = (progress->previous * width) >> 8;
    int current = (progress->current * width) >> 8;

    int delta = current - previous;
    if (delta != 0) {
      int step = Math_signum(delta);
      int color = delta > 0
        ? GBA_Bitmap8_GetPixel(bitmap, 2, 4)
        : GBA_Bitmap8_GetPixel(bitmap, 1, 4);

      for (int i = 0; i != delta; i += step) {
        GBA_TileMapRef_SetPixel(&target, px + current - i, py + 0, color);
        GBA_TileMapRef_SetPixel(&target, px + current - i, py + 1, color);
      }

      GBA_TileMapRef_SetPixel(&target, px + current - delta, py + 0, color);
      GBA_TileMapRef_SetPixel(&target, px + current - delta, py + 1, color);
    }
  }

  progress->redraw = false;
}
