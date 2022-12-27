
#include <game/object.h>

typedef struct Properties {
  int height;
} align4 Properties;

bool
Object_CreateBoxWithPole(
    Object *object,
    int height)
{
  int size = --height * 8;
  if (height < 0) {
    return false;
  }

  Bounds hitbox  = Bounds_Of(8, 32 + size, 8,  8 + size);
  Bounds viewbox = Bounds_Of(8, 20 + size, 8, 20 + size);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = false;
  object->type = TYPE_BOX_WITH_POLE;

  Properties *props = Object_GetProperties(object);
  props->height = height;

  return true;
}

static const GBA_TileMapRef pole = {
  .width = 2, .height = 5,
  .tiles = (GBA_Tile[]) {
    // pole
    { .tileId = 22, .vFlip = 0, .hFlip = 0 },
    { .tileId = 23, .vFlip = 0, .hFlip = 0 },
    { .tileId = 30, .vFlip = 0, .hFlip = 0 },
    { .tileId = 31, .vFlip = 0, .hFlip = 0 },
    { .tileId = 38, .vFlip = 0, .hFlip = 0 },
    { .tileId = 39, .vFlip = 0, .hFlip = 0 },
    // block
    { .tileId = 18, .vFlip = 0, .hFlip = 0 },
    { .tileId = 18, .vFlip = 0, .hFlip = 1 },
    { .tileId = 26, .vFlip = 0, .hFlip = 0 },
    { .tileId = 26, .vFlip = 0, .hFlip = 1 },
  }
};

static const GBA_TileMapRef vbox[] = {
  // top
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 19, .vFlip = 0, .hFlip = 0 },
      { .tileId = 19, .vFlip = 0, .hFlip = 1 },
      { .tileId = 27, .vFlip = 1, .hFlip = 0 },
      { .tileId = 27, .vFlip = 1, .hFlip = 1 },
    }
  },
  // middle
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 27, .vFlip = 0, .hFlip = 0 },
      { .tileId = 27, .vFlip = 0, .hFlip = 1 },
      { .tileId = 27, .vFlip = 1, .hFlip = 0 },
      { .tileId = 27, .vFlip = 1, .hFlip = 1 },
    }
  },
  // bottom
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 27, .vFlip = 0, .hFlip = 0 },
      { .tileId = 27, .vFlip = 0, .hFlip = 1 },
      { .tileId = 35, .vFlip = 0, .hFlip = 0 },
      { .tileId = 35, .vFlip = 0, .hFlip = 1 },
    }
  }
};

void
Object_DrawBoxWithPole(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  const GBA_TileMapRef *tiles = &pole;
  GBA_TileMapRef_Blit(target, tx, ty, &pole);
  ty += tiles->height;

  Properties *props = Object_GetProperties(object);
  int height = props->height;

  if (height-- > 0) {
    for (int y = 0; y < height; y++) {
      const GBA_TileMapRef *tiles = &vbox[y != 0];
      GBA_TileMapRef_Blit(target, tx, ty, tiles);
      ty += tiles->height;
    }

    GBA_TileMapRef_Blit(target, tx, ty, &vbox[2]);
  }
}
