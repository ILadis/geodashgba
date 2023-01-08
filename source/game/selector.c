
#include <game/selector.h>

Selector*
Selector_GetInstance(bool redraw) {
  static Selector selector = {0};

  if (redraw) {
    selector.redraw = true;
  }

  return &selector;
}

static inline void
Selector_Goto(
    Selector *selector,
    Direction direction)
{
  LevelId next = selector->id + (direction == DIRECTION_RIGHT ? +1 : -1);

 if (next >= 0 && next < LEVEL_COUNT) {
    selector->id = next;
  }
}

void
Selector_GoForward(Selector *selector) {
  Selector_Goto(selector, DIRECTION_RIGHT);
}

void
Selector_GoBackward(Selector *selector) {
  Selector_Goto(selector, DIRECTION_LEFT);
}

void
Selector_Update(Selector *selector) {
  // TODO
}

static inline void
Selector_DrawOverlay(Selector *selector) {
  extern const GBA_TileMapRef selectTopOverlayTileMap;
  extern const GBA_TileMapRef selectLeftEdgeOverlayTileMap;
  extern const GBA_TileMapRef selectRightEdgeOverlayTileMap;

  static const GBA_BackgroundControl layers[] = {
    { // level layer
      .size = 1, // 512x256
      .colorMode = 1,
      .tileSetIndex = 0,
      .tileMapIndex = 21,
      .priority = 1,
    },
    { // overlay layer
      .size = 0, // 256x256
      .colorMode = 1,
      .tileSetIndex = 0,
      .tileMapIndex = 23,
      .priority = 0,
    }
  };

  GBA_EnableBackgroundLayer(2, layers[0]);
  GBA_EnableBackgroundLayer(3, layers[1]);

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 3);

  GBA_TileMapRef_Blit(&target,  5,  0, &selectTopOverlayTileMap);
  GBA_TileMapRef_Blit(&target,  0, 16, &selectLeftEdgeOverlayTileMap);
  GBA_TileMapRef_Blit(&target, 25, 16, &selectRightEdgeOverlayTileMap);
}

static inline void
Selector_DrawLevelIndicator(Selector *selector) {
  const GBA_Tile active = { .tileId = 94 };
  const GBA_Tile inactive = { .tileId = 95 };

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 3);

  int ty = 5;
  int tx = (GBA_SCREEN_COLS - LEVEL_COUNT) / 2;

  for (LevelId id = 0; id < LEVEL_COUNT; id++) {
    const GBA_Tile *tile = id == selector->id ? &active : &inactive;
    GBA_TileMapRef_BlitTile(&target, tx++, ty, tile);
  }
}

static inline void
Selector_DrawLevelBox(
    Selector *selector,
    const Bounds *bounds)
{
  Vector lower = Bounds_Lower(bounds);
  Vector upper = Bounds_Upper(bounds);

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 2);

  const GBA_Tile tile = { .tileId = 79 };
  const GBA_Tile edges[] = {
    { .tileId = 71, .vFlip = 0, .hFlip = 0 }, // top left
    { .tileId = 71, .vFlip = 0, .hFlip = 1 }, // top right
    { .tileId = 71, .vFlip = 1, .hFlip = 0 }, // bottom left
    { .tileId = 71, .vFlip = 1, .hFlip = 1 }, // bottom right
  };

  int ty = lower.y;

  { // top
    int tx = lower.x;
    GBA_TileMapRef_BlitTile(&target, tx++, ty, &edges[0]);
    while (tx < upper.x - 1) GBA_TileMapRef_BlitTile(&target, tx++, ty, &tile);
    GBA_TileMapRef_BlitTile(&target, tx, ty, &edges[1]);
  }

  for (ty++; ty < upper.y - 1; ty++) {
    for (int tx = lower.x; tx < upper.x; tx++) {
      GBA_TileMapRef_BlitTile(&target, tx, ty, &tile);
    }
  }

  { // bottom
    int tx = lower.x;
    GBA_TileMapRef_BlitTile(&target, tx++, ty, &edges[2]);
    while (tx < upper.x - 1) GBA_TileMapRef_BlitTile(&target, tx++, ty, &tile);
    GBA_TileMapRef_BlitTile(&target, tx, ty, &edges[3]);
  }
}

void
Selector_Draw(Selector *selector) {
  if (selector->redraw) {
    Selector_DrawOverlay(selector);

    Bounds bounds1 = Bounds_Of(15, 9, 11, 3);
    Selector_DrawLevelBox(selector, &bounds1);

    Bounds bounds2 = Bounds_Of(15 + 32, 9, 11, 3);
    Selector_DrawLevelBox(selector, &bounds2);
  }

  Selector_DrawLevelIndicator(selector);
  selector->redraw = false;
}
