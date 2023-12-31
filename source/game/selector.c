
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

  if (visible) {
    // get color of tileId 79
    GBA_System *system = GBA_GetSystem();
    GBA_Bitmap8 *bitmap = &system->tileSets8[0][79];
    int background = GBA_Bitmap8_GetPixel(bitmap, 0, 0);

    extern const Font hudFont;

    Text *text = &selector->text;
    Text_SetFont(text, &hudFont);

    int fill = GBA_Palette_FindColor(GBA_Color_From(0xffffff)); // actual color is 0xf8f8f8
    int outline = GBA_Palette_FindColor(GBA_Color_From(0x000008));

    Text_SetFillColor(text, fill);
    Text_SetOutlineColor(text, outline);
    Text_SetBackgroundColor(text, background);
  }
  else {
    GBA_DisableBackgroundLayer(2);
    GBA_DisableBackgroundLayer(3);

    for (unsigned int i = 0; i < length(selector->arrows); i++) {
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
  int next = selector->index + vector->x;

  const Collection *collection = Collection_GetInstance();
  int count = Collection_GetLevelCount(collection);

 if (next >= 0 && next < count) {
    Animation *animation = &selector->scroll;
    Animation_Cancel(animation);

    int from = Animation_CurrentValue(animation);
    int to = from + vector->x * 512;

    Animation scroll = Animation_From(from, to, Timing_EaseOut);
    Animation_Start(&scroll);

    selector->scroll = scroll;
    selector->index = next;
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
  int last = Animation_CurrentValue(&selector->scroll);

  Animation_Tick(&selector->scroll, 4);
  int next = Animation_CurrentValue(&selector->scroll);

  int threshold = selector->scroll.from + (selector->scroll.to - selector->scroll.from)/2;
  bool overshoot = false
    || (last < threshold && next >= threshold)
    || (last > threshold && next <= threshold);

  if (overshoot) {
    selector->redraw = true;
  }

  if (!Animation_Tick(&selector->move, 6)) {
    Animation_Restart(&selector->move);
  }
}

static inline void
Selector_DrawOverlay(unused Selector *selector) {
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

  const Collection *collection = Collection_GetInstance();
  int count = Collection_GetLevelCount(collection);

  int ty = 5;
  int tx = (GBA_SCREEN_COLS - count) / 2;

  for (int index = 0; index < count; index++) {
    const GBA_Tile *tile = index == selector->index ? &active : &inactive;
    GBA_TileMapRef_BlitTile(&target, tx++, ty, tile);
  }
}

static inline void
Selector_DrawLevelBox(Selector *selector) {
  extern const GBA_TileMapRef selectLevelBoxTileMap;
  const Vector position = Vector_Of(4, 6);

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 2);
  GBA_TileMapRef_Blit(&target, position.x, position.y, &selectLevelBoxTileMap);

  Text *text = &selector->text;
  int color = text->colors.background;
  for (int tileId = 128; tileId <= 167; tileId++) {
    GBA_TileMapRef_FillTile(&target, tileId, color);
  }

  Level *level = Selector_GetLevel(selector);

  char name[15];
  Level_GetName(level, name);

  int width = Text_GetWidth(text, name);
  int dx = (160 - width) / 2;

  Text_SetCanvas(text, &target);
  Text_SetCursor(text, position.x * 8 + 8 + dx, position.y * 8 + 16);
  Text_WriteLine(text, name);
}

static inline void
Selector_DrawArrows(
    Selector *selector,
    Direction direction)
{
  static const GBA_Sprite base = {
    .colorMode = 0,
    .paletteBank = 0,
    .shape = GBA_SPRITE_SHAPE_OF(16, 32),
    .size = GBA_SPRITE_SIZE_OF(16, 32),
    .tileId = 8,
    .objMode = GBA_SPRITE_MODE_RENDER,
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

  static const Vector positions[] = {
    [DIRECTION_LEFT]  = Vector_Of(13, 56),
    [DIRECTION_RIGHT] = Vector_Of(211, 56),
  };

  const Vector *position = &positions[direction];
  GBA_Sprite_SetPosition(sprite, position->x + dx * sign->x, position->y);

  const Collection *collection = Collection_GetInstance();
  int count = Collection_GetLevelCount(collection);

  const int bounds[] = {
    [DIRECTION_LEFT]  = 0,
    [DIRECTION_RIGHT] = count - 1,
  };

  bool visible = selector->index != bounds[direction];
  GBA_Sprite_SetObjMode(sprite, visible ? GBA_SPRITE_MODE_RENDER : GBA_SPRITE_MODE_HIDE);
}

void
Selector_Draw(Selector *selector) {
  Selector_DrawArrows(selector, DIRECTION_LEFT);
  Selector_DrawArrows(selector, DIRECTION_RIGHT);

  if (selector->redraw) {
    Selector_DrawOverlay(selector);
    Selector_DrawLevelBox(selector);
    Selector_DrawLevelIndicator(selector);
  }

  int scroll = Animation_CurrentValue(&selector->scroll);
  GBA_OffsetBackgroundLayer(2, scroll, 0);

  selector->redraw = false;
}
