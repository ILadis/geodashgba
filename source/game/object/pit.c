
#include <game/object.h>

typedef struct Properties {
  int width;
  enum Variant {
    PIT_VARIANT_REGULAR,
    PIT_VARIANT_HANGING,
  } variant;
} align4 Properties;

bool
Object_CreatePit(
    Object *object,
    int width,
    bool hanging)
{
  if (width < 1) {
    return false;
  }

  int x = width * 8;

  Bounds hitbox  = Bounds_Of(x, 6, x, 2);
  Bounds viewbox = Bounds_Of(x, 4, x, 4);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = true;
  object->type = OBJECT_TYPE_PIT;

  Properties *props = Object_GetProperties(object);
  props->width = width;
  props->variant = hanging ? PIT_VARIANT_HANGING : PIT_VARIANT_REGULAR;

  return true;
}

static const GBA_TileMapRef pit[] = {
  [PIT_VARIANT_REGULAR] = {
    .width = 16, .height = 1,
    .tiles = GBA_Tiles_Of(
      {.tileId =  5, .vFlip = 0, .hFlip = 0 },
      {.tileId =  6, .vFlip = 0, .hFlip = 0 },
      {.tileId =  7, .vFlip = 0, .hFlip = 0 },
      {.tileId = 13, .vFlip = 0, .hFlip = 0 },
      {.tileId = 14, .vFlip = 0, .hFlip = 0 },
      {.tileId = 15, .vFlip = 0, .hFlip = 0 },
      {.tileId = 15, .vFlip = 0, .hFlip = 1 },
      {.tileId = 14, .vFlip = 0, .hFlip = 1 },
      {.tileId = 13, .vFlip = 0, .hFlip = 1 },
      {.tileId =  7, .vFlip = 0, .hFlip = 1 },
      {.tileId =  6, .vFlip = 0, .hFlip = 1 },
      {.tileId =  5, .vFlip = 0, .hFlip = 1 },
      {.tileId =  7, .vFlip = 0, .hFlip = 0 },
      {.tileId = 13, .vFlip = 0, .hFlip = 0 },
      {.tileId =  6, .vFlip = 0, .hFlip = 0 },
      {.tileId =  5, .vFlip = 0, .hFlip = 0 },
    )
  },
  [PIT_VARIANT_HANGING] = {
    .width = 16, .height = 1,
    .tiles = GBA_Tiles_Of(
      {.tileId =  5, .vFlip = 1, .hFlip = 0 },
      {.tileId =  6, .vFlip = 1, .hFlip = 0 },
      {.tileId =  7, .vFlip = 1, .hFlip = 0 },
      {.tileId = 13, .vFlip = 1, .hFlip = 0 },
      {.tileId = 14, .vFlip = 1, .hFlip = 0 },
      {.tileId = 15, .vFlip = 1, .hFlip = 0 },
      {.tileId = 15, .vFlip = 1, .hFlip = 1 },
      {.tileId = 14, .vFlip = 1, .hFlip = 1 },
      {.tileId = 13, .vFlip = 1, .hFlip = 1 },
      {.tileId =  7, .vFlip = 1, .hFlip = 1 },
      {.tileId =  6, .vFlip = 1, .hFlip = 1 },
      {.tileId =  5, .vFlip = 1, .hFlip = 1 },
      {.tileId =  7, .vFlip = 1, .hFlip = 0 },
      {.tileId = 13, .vFlip = 1, .hFlip = 0 },
      {.tileId =  6, .vFlip = 1, .hFlip = 0 },
      {.tileId =  5, .vFlip = 1, .hFlip = 0 },
    )
  }
};

void
Object_DrawPit(
    Object *object,
    GBA_TileMapRef *target)
{
  Properties *props = Object_GetProperties(object);
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  const GBA_TileMapRef *tileMap = &pit[props->variant];

  int width = props->width * 2;
  for (int x = 0; x < width; x++) {
    const GBA_Tile *tile = &tileMap->tiles[x % 16];
    GBA_TileMapRef_BlitTile(target, tx + x, ty, tile);
  }
}
