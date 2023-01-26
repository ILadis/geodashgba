
#include <game/selector.h>

Selector*
Selector_GetInstance() {
  static Selector selector = {0};
  return &selector;
}

void
Selector_SetVisible(
    Selector *selector,
    bool visible)
{
  selector->redraw = visible;

  if (!visible) {
    for (int i = 0; i < length(selector->arrows); i++) {
      GBA_Sprite *sprite = selector->arrows[i];
      if (sprite != NULL) {
        GBA_Sprite_Release(sprite);
        selector->arrows[i] = NULL;
      }
    }
  }
}

static inline void
Selector_Goto(
    Selector *selector,
    Direction direction)
{
  const Vector *vector = Vector_FromDirection(direction);
  LevelId next = selector->id + vector->x;

 if (next >= 0 && next < LEVEL_COUNT) {
    Animation *animation = &selector->scroll;
    Animation_Cancel(animation);

    int from = Animation_CurrentValue(animation);
    int to = from + vector->x * 256;

    Animation scroll = Animation_From(from, to, Timing_EaseOut);
    Animation_Start(&scroll);

    selector->scroll = scroll;
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
  Animation_Tick(&selector->scroll, 4);

  if (!Animation_Tick(&selector->move, 6)) {
    Animation_Restart(&selector->move);
  }
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

  static bool flag = true;
  if (flag) {
    extern const Font hudFont;

    Printer *printer = &selector->printer;
    Printer_SetFont(printer, &hudFont);
    Printer_SetCanvas(printer, &target);
    Printer_SetCursor(printer, lower.x * 8 + 8, lower.y * 8 + 8);
    Printer_PutChar(printer, 3, 2);

    flag = false;
  }
}

static inline void
Selector_DrawArrows(
    Selector *selector,
    Direction direction)
{
  static const GBA_Sprite base = {
    .colorMode = 0,
    .paletteBank = 0,
    .shape = 2, // 16x32
    .size  = 2,
    .tileId = 8,
    .gfxMode = 0,
    .objMode = 0,
    .priority = 0,
  };

  GBA_Sprite *sprite = selector->arrows[direction];

  if (sprite == NULL) {
    sprite = GBA_Sprite_Allocate();
    sprite->attr0 = base.attr0;
    sprite->attr1 = base.attr1;
    sprite->attr2 = base.attr2;

    Animation move = Animation_From(0, 8, Timing_EaseOut);
    Animation_Start(&move);

    selector->move = move;
    selector->arrows[direction] = sprite;

    GBA_Sprite_SetHFlip(sprite, direction == DIRECTION_RIGHT);
  }

  const Vector *sign = Vector_FromDirection(direction);
  int dx = Animation_CurrentValue(&selector->move);
  dx *= sign->x;

  static const Vector positions[] = {
    [DIRECTION_LEFT]  = Vector_Of(13, 56),
    [DIRECTION_RIGHT] = Vector_Of(211, 56),
  };

  const Vector *position = &positions[direction];
  GBA_Sprite_SetPosition(sprite, position->x + dx, position->y);

  static const LevelId bounds[] = {
    [DIRECTION_LEFT]  = 0,
    [DIRECTION_RIGHT] = LEVEL_COUNT - 1,
  };

  bool visible = selector->id != bounds[direction];
  GBA_Sprite_SetObjMode(sprite, visible ? 0 : 2);
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

  Selector_DrawArrows(selector, DIRECTION_LEFT);
  Selector_DrawArrows(selector, DIRECTION_RIGHT);
  Selector_DrawLevelIndicator(selector);

  int scroll = Animation_CurrentValue(&selector->scroll);
  GBA_OffsetBackgroundLayer(2, scroll, 0);

  selector->redraw = false;
}
