
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
  int length = width * count;

  progress->length = length;
}

void
Progress_Update(
    Progress *progress,
    Cube *cube)
{
  const Vector *position = Cube_GetPosition(cube);

  int value = Math_div(position->x << 8, progress->length);
  value = Math_min(value, 1 << 8);

  progress->value = value;
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

  const int width = 123;
  const int tx = 7, ty = 1;

  if (progress->redraw) {
    GBA_EnableBackgroundLayer(2, layer);

    GBA_TileMapRef target;
    GBA_TileMapRef_FromBackgroundLayer(&target, 2);

    GBA_Tile tile = { .tileId = 48 };
    for (int i = 0; i < 16; i++) {
      GBA_TileMapRef_BlitTile(&target, tx + i, ty, &tile);
      tile.tileId++;
    }
  }

  // TODO improve/optimize this
  else {
    GBA_TileMapRef target;
    GBA_TileMapRef_FromBackgroundLayer(&target, 2);

    GBA_System *system = GBA_GetSystem();
    GBA_Bitmap8 *bitmap = &system->tileSets8[0][48];

    int active = GBA_Bitmap8_GetPixel(bitmap, 2, 4);
    int inactive = GBA_Bitmap8_GetPixel(bitmap, 1, 4);

    int px = 3 + tx * 8;
    int py = 3 + ty * 8;
  
    int length = (progress->value * width) >> 8;

    for (int i = 0; i < width; i++) {
      GBA_TileMapRef_SetPixel(&target, px + i, py + 0, i < length ? active : inactive);
      GBA_TileMapRef_SetPixel(&target, px + i, py + 1, i < length ? active : inactive);
    }
  }

  progress->redraw = false;
}
