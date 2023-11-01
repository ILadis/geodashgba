
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
    [MODE_SELECT] = Vector_Of(7, 12),
  };

  return &positions[progress->mode];
}

void
Progress_SetCourse(
    Progress *progress,
    Course *course)
{
  progress->redraw = true;

  const Bounds *bounds = Course_GetBounds(course);
  int total = bounds->size.x * 2 - 16; // levels should always end with a wall with a width of 8px

  progress->total = total;
  progress->best = 0;
}

void
Progress_SetProgress(
    Progress *progress,
    int value)
{
  progress->current = Math_min(value, PROGRESS_MAX_VALUE);
  progress->best = Math_max(progress->best, progress->current);
}

void
Progress_SetCollectedCoin(
    Progress *progress,
    unsigned int index)
{
  if (index < length(progress->coins)) {
    progress->coins[index] = true;
  }
}

void
Progress_SetCollectedCoins(
    Progress *progress,
    const bool *coins)
{
  progress->redraw = progress->mode == MODE_SELECT;

  for (unsigned int i = 0; i < length(progress->coins); i++) {
    progress->coins[i] = coins[i];
  }
}

void
Progress_Update(
    Progress *progress,
    Cube *cube)
{
  const Vector *position = Cube_GetPosition(cube);

  if (!cube->success && Cube_EnteredState(cube, STATE_DESTROYED)) {
    progress->coins[0] = false;
    progress->coins[1] = false;
    progress->coins[2] = false;
  }

  int value = Math_div(position->x * PROGRESS_MAX_VALUE, progress->total);
  Progress_SetProgress(progress, value);
}

static inline void
Progress_DrawBarPixels(
    Progress *progress,
    int index)
{
  const Vector widths[][3] = {
    [MODE_PLAY]   = { Vector_Of(2, 8), Vector_Of(0, 8), Vector_Of(0, 6) },
    [MODE_SELECT] = { Vector_Of(1, 8), Vector_Of(0, 8), Vector_Of(0, 7) },
  };

  const Vector heights[][3][8] = {
    [MODE_PLAY] = {
      { Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5) },
      { Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5) },
      { Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5), Vector_Of(3, 5) },
    },
    [MODE_SELECT] = {
      { Vector_Of(0,  0), Vector_Of(8, 13), Vector_Of(7, 14), Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15) },
      { Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15) },
      { Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(6, 15), Vector_Of(7, 14), Vector_Of(8, 13), Vector_Of(0,  0) },
    }
  };

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 2);

  const Vector *position = Progress_GetPosition(progress);
  const int px = position->x * 8 + index * 8;
  const int py = position->y * 8;

  const Vector *width = NULL;
  switch (index) {
    case  0: width = &widths[progress->mode][0]; break;
    case 15: width = &widths[progress->mode][2]; break;
    default: width = &widths[progress->mode][1]; break;
  }

  const Vector *height = NULL;
  switch (index) {
    case  0: height = heights[progress->mode][0]; break;
    case 15: height = heights[progress->mode][2]; break;
    default: height = heights[progress->mode][1]; break;
  }

  int value = index * PROGRESS_PER_TILE;

  for (int x = width->x; x < width->y; x++) {
    bool state = progress->current > value;
    int color = progress->colors[state];

    for (int y = height[x].x; y < height[x].y; y++) {
      GBA_TileMapRef_SetPixel(&target, px + x, py + y, color);
    }

    value += PROGRESS_PER_PIXEL;
  }
}

static const GBA_TileMapRef bars[][5] = {
  [MODE_PLAY] = {
    { .width = 1, .height = 1, .tiles = (GBA_Tile[]) { { .tileId = 48, } } },
    { .width = 1, .height = 1, .tiles = (GBA_Tile[]) { { .tileId = 49, } } },
    { .width = 1, .height = 1, .tiles = (GBA_Tile[]) { { .tileId = 50, } } },
    { .width = 1, .height = 1, .tiles = (GBA_Tile[]) { { .tileId = 51, } } },
    { .width = 1, .height = 1, .tiles = (GBA_Tile[]) { { .tileId = 52, } } },
  },
  [MODE_SELECT] = {
    { .width = 1, .height = 2, .tiles = (GBA_Tile[]) { { .tileId =  96 }, { .tileId = 104 }, } },
    { .width = 1, .height = 2, .tiles = (GBA_Tile[]) { { .tileId =  97 }, { .tileId = 105 }, } },
    { .width = 1, .height = 2, .tiles = (GBA_Tile[]) { { .tileId =  98 }, { .tileId = 106 }, } },
    { .width = 1, .height = 2, .tiles = (GBA_Tile[]) { { .tileId =  99 }, { .tileId = 107 }, } },
    { .width = 1, .height = 2, .tiles = (GBA_Tile[]) { { .tileId = 100 }, { .tileId = 108 }, } },
  }
};

static inline void
Progress_DrawBarTiles(
    Progress *progress,
    int index)
{
  int lower = index * PROGRESS_PER_TILE;
  int upper = lower + PROGRESS_PER_TILE;

  bool progressing = progress->current > lower && progress->current < upper;
  bool completed = progress->current >= upper;

  const GBA_TileMapRef *tileMap = NULL;
  switch (index) {
    case  0: tileMap = &bars[progress->mode][0]; progressing = true; break;
    case 15: tileMap = &bars[progress->mode][4]; progressing = true; break;
    default: tileMap = &bars[progress->mode][progressing ? 2 : completed ? 3 : 1]; break;
  }

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 2);

  const Vector *position = Progress_GetPosition(progress);
  GBA_TileMapRef_Blit(&target, position->x + index, position->y, tileMap);

  if (progressing) {
    Progress_DrawBarPixels(progress, index);
  }
}

static inline void
Progress_DrawDelta(Progress *progress) {
  int current = progress->current;
  int delta = current - progress->previous;

  if (delta != 0) {
    int step = Math_signum(delta);

    int index = (current - delta) >> 4;
    int target = Math_min(15, current >> 4);

    for (; index != target; index += step) {
      Progress_DrawBarTiles(progress, index);
    }

    Progress_DrawBarTiles(progress, index);
    progress->previous = current;
  }
}

static inline void
Progress_DrawBar(Progress *progress) {
  static const GBA_BackgroundControl layers[] = {
    [MODE_PLAY] = {
      .size = 0, // 256x256
      .colorMode = 1,
      .tileSetIndex = 0,
      .tileMapIndex = 20,
      .priority = 1,
    },
    // overlay layer (shared with selector)
    [MODE_SELECT] = {
      .size = 1, // 512x256
      .colorMode = 1,
      .tileSetIndex = 0,
      .tileMapIndex = 21,
      .priority = 1,
    }
  };

  // store "on" and "off" colors
  progress->colors[true]  = GBA_Palette_FindColor(GBA_Color_From(0x7BFF00));
  progress->colors[false] = GBA_Palette_FindColor(GBA_Color_From(0x10006B));

  GBA_EnableBackgroundLayer(2, layers[progress->mode]);

  for (int index = 0; index < 16; index++) {
    Progress_DrawBarTiles(progress, index);
  }

  int current = progress->current;
  progress->previous = current;
}

static const GBA_TileMapRef coin[] = {
  // collected
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 174, .vFlip = 0, .hFlip = 0 },
      { .tileId = 175, .vFlip = 0, .hFlip = 0 },
      { .tileId = 182, .vFlip = 0, .hFlip = 0 },
      { .tileId = 183, .vFlip = 0, .hFlip = 0 },
    }
  },
  // not collected
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 206, .vFlip = 0, .hFlip = 0 },
      { .tileId = 207, .vFlip = 0, .hFlip = 0 },
      { .tileId = 214, .vFlip = 0, .hFlip = 0 },
      { .tileId = 215, .vFlip = 0, .hFlip = 0 },
    }
  }
};

static inline void
Progress_DrawCoins(Progress *progress) {
  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 2);

  int tx = 19;
  int ty = 10;

  for (unsigned int i = 0; i < length(progress->coins); i++) {
    bool index = progress->coins[i] ? 0 : 1;
    const GBA_TileMapRef *tiles = &coin[index];
    GBA_TileMapRef_Blit(&target, tx + i * tiles->width, ty, tiles);
  }
}

void
Progress_Draw(Progress *progress) {
  if (progress->redraw) {
    Progress_DrawBar(progress);

    if (progress->mode == MODE_SELECT) {
      Progress_DrawCoins(progress);
    }
  }

  Progress_DrawDelta(progress);

  progress->redraw = false;
}
