
#include <game/object.h>

typedef struct Properties {
  int width;
  // TODO adjustable launch height
} align4 Properties;

bool
Object_CreatePit(
    Object *object,
    int width)
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
  object->type = TYPE_PIT;

  Properties *props = Object_GetProperties(object);
  props->width = width;

  return true;
}

static const GBA_TileMapRef pit = {
  .width = 16, .height = 1,
  .tiles = (GBA_Tile[]) {
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

  int width = props->width * 2;
  for (int x = 0; x < width; x++) {
    const GBA_Tile *tile = &pit.tiles[x % 16];
    GBA_TileMapRef_BlitTile(target, tx + x, ty, tile);
  }
}
