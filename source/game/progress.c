
#include <game/progress.h>

Progress*
Progress_GetInstance() {
  static Progress progress = {0};
  return &progress;
}

static inline const Vector*
Progress_GetPosition(Progress *progress) {
  static const Vector positions[] = {
    [MODE_PLAY] = Vector_Of(7, 1),
    [MODE_SELECTOR] = Vector_Of(7, 12),
  };

  return &positions[progress->mode];
}

static inline const GBA_TileMapRef*
Progress_GetTileMap(Progress *progress) {
  extern const GBA_TileMapRef progressPlayTileMap;
  extern const GBA_TileMapRef progressSelectorTileMap;

  static const GBA_TileMapRef *tileMaps[] = {
    [MODE_PLAY] = &progressPlayTileMap,
    [MODE_SELECTOR] = &progressSelectorTileMap,
  };

  return tileMaps[progress->mode];
}

void
Progress_SetCourse(
    Progress *progress,
    Course *course)
{
  progress->redraw = true;

  const Bounds *bounds = Course_GetBounds(course);
  int total = bounds->size.x * 2 - 8; // levels should always end with a wall with a width of 8px

  progress->total = total;
}

void
Progress_Update(
    Progress *progress,
    Cube *cube)
{
  const Vector *position = Cube_GetPosition(cube);

  int value = Math_div(position->x << 8, progress->total);
  Progress_SetProgress(progress, value);
}

static inline void
Progress_DrawPixels(
    Progress *progress,
    int value, bool state)
{
  const Vector dimensions[] = {
    [MODE_PLAY] = Vector_Of(122, 2),
    [MODE_SELECTOR] = Vector_Of(125, 9),
  };

  const Vector offsets[] = {
    [MODE_PLAY] = Vector_Of(2, 3),
    [MODE_SELECTOR] = Vector_Of(1, 6),
  };

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 2);

  GBA_System *system = GBA_GetSystem();
  GBA_Bitmap8 *bitmap = &system->tileSets8[0][state ? 80: 79];

  int color = GBA_Bitmap8_GetPixel(bitmap, 0, 0);

  const Vector *position = Progress_GetPosition(progress);

  int width = (dimensions[progress->mode].x * value) >> 8;
  int height = dimensions[progress->mode].y;

  int px = offsets[progress->mode].x + position->x * 8 + width;
  int py = offsets[progress->mode].y + position->y * 8;

  // TODO workaround to achieve rounded corners
  if (progress->mode == MODE_SELECTOR) {
    switch (width) {
      case   0: py += 2; height = 5; break;
      case   1: py += 1; height = 7; break;
      case 124: py += 1; height = 7; break;
      case 125: py += 2; height = 5; break;
    }
  }

  for (int dy = 0; dy < height; dy++) {
    GBA_TileMapRef_SetPixel(&target, px, py + dy, color);
  }
}

static inline void
Progress_DrawDelta(Progress *progress) {
  int current = progress->current;
  int delta = current - progress->previous;
  if (delta != 0) {
    int step = Math_signum(delta);
    bool state = step > 0;

    int value = current;
    for (int i = 0; i != delta; i += step, value -= step) {
      Progress_DrawPixels(progress, value, state);
    }

    Progress_DrawPixels(progress, value, state);
  }

  progress->previous = current;
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

  // TODO worarkound to share background layer 2 when on selector scene
  if (progress->mode == MODE_PLAY) {
    GBA_EnableBackgroundLayer(2, layer);
  }

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 2);

  const Vector *position = Progress_GetPosition(progress);
  const GBA_TileMapRef *tileMap = Progress_GetTileMap(progress);

  GBA_TileMapRef_Blit(&target, position->x, position->y, tileMap);

  int current = progress->current;
  for (int value = 0; value < 1 << 8; value++) {
    Progress_DrawPixels(progress, value, value < current);
  }

  progress->previous = current;
}

void
Progress_Draw(Progress *progress) {
  if (progress->redraw) {
    Progress_DrawBar(progress);
  }

  Progress_DrawDelta(progress);

  progress->redraw = false;
}
