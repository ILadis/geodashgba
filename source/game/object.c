
#include <game/object.h>

static inline void
Object_SetViewboxFromTiles(Object *object) {
  const GBA_TileMapRef *tiles = object->tiles;

  int x = (tiles->width  * 8) / 2;
  int y = (tiles->height * 8) / 2;

  Bounds viewbox = Bounds_Of(x, y, x, y);

  object->viewbox = viewbox;
}

static const GBA_TileMapRef box = (GBA_TileMapRef) {
  .width = 2,
  .height = 2,
  .tiles = (GBA_Tile[]) {
    { .tileId = 19, .vFlip = 0, .hFlip = 0 },
    { .tileId = 19, .vFlip = 0, .hFlip = 1 },
    { .tileId = 27, .vFlip = 0, .hFlip = 0 },
    { .tileId = 27, .vFlip = 0, .hFlip = 1 },
  }
};

void
Object_CreateBox(Object *object) {
  object->hitbox = Bounds_Of(8, 8, 8, 8);
  object->tiles = &box;
  object->solid = true;
  object->deadly = false;
  Object_SetViewboxFromTiles(object);
}

static const GBA_TileMapRef blockWithPole = (GBA_TileMapRef) {
  .width = 2,
  .height = 5,
  .tiles = (GBA_Tile[]) {
    { .tileId = 37, .vFlip = 0, .hFlip = 0 },
    { .tileId = 38, .vFlip = 0, .hFlip = 0 },
    { .tileId = 45, .vFlip = 0, .hFlip = 0 },
    { .tileId = 46, .vFlip = 0, .hFlip = 0 },
    { .tileId = 53, .vFlip = 0, .hFlip = 0 },
    { .tileId = 54, .vFlip = 0, .hFlip = 0 },
    { .tileId = 18, .vFlip = 0, .hFlip = 0 },
    { .tileId = 18, .vFlip = 0, .hFlip = 1 },
    { .tileId = 26, .vFlip = 0, .hFlip = 0 },
    { .tileId = 26, .vFlip = 0, .hFlip = 1 },
  }
};

void
Object_CreateBoxWithPole(
    Object *object,
    int height)
{
  // TODO implement different sizes (heights)
  object->hitbox = Bounds_Of(8, 32, 8, 8);
  object->tiles = &blockWithPole;
  object->solid = true;
  object->deadly = false;
  Object_SetViewboxFromTiles(object);
}

static const GBA_TileMapRef spike = (GBA_TileMapRef) {
  .width = 2,
  .height = 2,
  .tiles = (GBA_Tile[]) {
    { .tileId = 17, .vFlip = 0, .hFlip = 0 },
    { .tileId = 17, .vFlip = 0, .hFlip = 1 },
    { .tileId = 25, .vFlip = 0, .hFlip = 0 },
    { .tileId = 25, .vFlip = 0, .hFlip = 1 },
  }
};

void
Object_CreateSpike(
    Object *object,
    Direction direction)
{
  // TODO implement different directions
  object->hitbox = Bounds_Of(8, 8, 8, 8);
  object->tiles = &spike;
  object->solid = true;
  object->deadly = true;
  Object_SetViewboxFromTiles(object);
}
