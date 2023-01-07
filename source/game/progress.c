
#include <game/progress.h>

Progress*
Progress_GetInstance() {
  static Progress progress = {0};
  return &progress;
}

static inline const Vector*
Progress_GetPosition(Progress *progress) {
  static const Vector position = Vector_Of(7, 1);
  return &position;
}

void
Progress_SetCourse(
    Progress *progress,
    Course *course)
{
  progress->redraw = true;

  const Bounds *bounds = Course_GetBounds(course);
  int total = bounds->size.x * 2;

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

static inline void
Progress_DrawDelta(Progress *progress) {
  const int width = 122;

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 2);

  GBA_System *system = GBA_GetSystem();
  GBA_Bitmap8 *bitmap = &system->tileSets8[0][48];

  int previous = (progress->previous * width) >> 8;
  int current = (progress->current * width) >> 8;

  int delta = current - previous;
  if (delta != 0) {
    int step = Math_signum(delta);
    int color = delta > 0
      ? GBA_Bitmap8_GetPixel(bitmap, 2, 4)
      : GBA_Bitmap8_GetPixel(bitmap, 1, 4);

    const Vector *position = Progress_GetPosition(progress);

    int px = 3 + position->x * 8 + current;
    int py = 3 + position->y * 8;

    for (int i = 0; i != delta; i += step, px -= step) {
      GBA_TileMapRef_SetPixel(&target, px, py + 0, color);
      GBA_TileMapRef_SetPixel(&target, px, py + 1, color);
    }

    GBA_TileMapRef_SetPixel(&target, px, py + 0, color);
    GBA_TileMapRef_SetPixel(&target, px, py + 1, color);
  }
}

static inline void
Progress_DrawBar(Progress *progress) {
  static const GBA_BackgroundControl layer = {
    .size = 0, // 256x256
    .colorMode = 1,
    .tileSetIndex = 0,
    .tileMapIndex = 20,
    .priority = 1,
  };

  if (progress->redraw) {
    GBA_EnableBackgroundLayer(2, layer);
    GBA_DisableBackgroundLayer(3);

    GBA_TileMapRef target;
    GBA_TileMapRef_FromBackgroundLayer(&target, 2);

    const Vector *position = Progress_GetPosition(progress);

    GBA_Tile tile = { .tileId = 48 };
    for (int i = 0; i < 16; i++) {
      GBA_TileMapRef_BlitTile(&target, position->x + i, position->y, &tile);
      tile.tileId++;
    }
  }

  progress->redraw = false;
}

void
Progress_Draw(Progress *progress) {
  Progress_DrawBar(progress);
  Progress_DrawDelta(progress);
}
