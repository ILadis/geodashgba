
#include <game/object.h>

static inline void
Object_SetViewboxFromTiles(Object *object) {
  const GBA_TileMapRef *tiles = object->tiles;

  int x = (tiles->width  >> 1) * 8;
  int y = (tiles->width  >> 1) * 8;

  int width =  x + 4;
  int height = y + 4;

  Bounds viewbox = Bounds_Of(x, y, width, height);

  object->viewbox = viewbox;
}

static const GBA_TileMapRef box = (GBA_TileMapRef) {
  .width = 2,
  .height = 2,
  .tiles = (GBA_Tile[]) {
    {.tileId = 19, .vFlip = 0, .hFlip = 0 },
    {.tileId = 19, .vFlip = 0, .hFlip = 1 },
    {.tileId = 27, .vFlip = 0, .hFlip = 0 },
    {.tileId = 27, .vFlip = 0, .hFlip = 1 },
  }
};

void
Object_CreateBox(Object *object) {
  object->hitbox = Bounds_Of(8, 8, 8, 9);
  object->tiles = &box;
  Object_SetViewboxFromTiles(object);
}

static const GBA_TileMapRef blockWpole = (GBA_TileMapRef) {
  .width = 2,
  .height = 5,
  .tiles = (GBA_Tile[]) {
    {.tileId = 37, .vFlip = 0, .hFlip = 0 },
    {.tileId = 38, .vFlip = 0, .hFlip = 0 },
    {.tileId = 45, .vFlip = 0, .hFlip = 0 },
    {.tileId = 46, .vFlip = 0, .hFlip = 0 },
    {.tileId = 53, .vFlip = 0, .hFlip = 0 },
    {.tileId = 54, .vFlip = 0, .hFlip = 0 },
    {.tileId = 18, .vFlip = 0, .hFlip = 0 },
    {.tileId = 18, .vFlip = 0, .hFlip = 1 },
    {.tileId = 26, .vFlip = 0, .hFlip = 0 },
    {.tileId = 26, .vFlip = 0, .hFlip = 1 },
  }
};

void
Object_CreateBlockWithPole(Object *object) {
  object->hitbox = Bounds_Of(8, 32, 8, 9);
  object->tiles = &blockWpole;
  Object_SetViewboxFromTiles(object);
}

static const GBA_TileMapRef platform = (GBA_TileMapRef) {
  .width = 10,
  .height = 3,
  .tiles = (GBA_Tile[]) {
    {.tileId = 24, .vFlip = 0, .hFlip = 0 },
    {.tileId = 24, .vFlip = 0, .hFlip = 0 },
    {.tileId = 24, .vFlip = 0, .hFlip = 0 },
    {.tileId = 24, .vFlip = 0, .hFlip = 0 },
    {.tileId = 24, .vFlip = 0, .hFlip = 0 },
    {.tileId = 24, .vFlip = 0, .hFlip = 0 },
    {.tileId = 24, .vFlip = 0, .hFlip = 0 },
    {.tileId = 24, .vFlip = 0, .hFlip = 0 },
    {.tileId = 24, .vFlip = 0, .hFlip = 0 },
    {.tileId = 24, .vFlip = 0, .hFlip = 0 },
    {.tileId = 19, .vFlip = 0, .hFlip = 0 },
    {.tileId = 20, .vFlip = 0, .hFlip = 0 },
    {.tileId = 21, .vFlip = 0, .hFlip = 0 },
    {.tileId = 21, .vFlip = 0, .hFlip = 0 },
    {.tileId = 21, .vFlip = 0, .hFlip = 0 },
    {.tileId = 21, .vFlip = 0, .hFlip = 0 },
    {.tileId = 21, .vFlip = 0, .hFlip = 0 },
    {.tileId = 21, .vFlip = 0, .hFlip = 0 },
    {.tileId = 20, .vFlip = 0, .hFlip = 1 },
    {.tileId = 19, .vFlip = 0, .hFlip = 1 },
    {.tileId = 27, .vFlip = 0, .hFlip = 0 },
    {.tileId = 28, .vFlip = 0, .hFlip = 0 },
    {.tileId = 29, .vFlip = 0, .hFlip = 0 },
    {.tileId = 29, .vFlip = 0, .hFlip = 0 },
    {.tileId = 29, .vFlip = 0, .hFlip = 0 },
    {.tileId = 29, .vFlip = 0, .hFlip = 0 },
    {.tileId = 29, .vFlip = 0, .hFlip = 0 },
    {.tileId = 29, .vFlip = 0, .hFlip = 0 },
    {.tileId = 28, .vFlip = 0, .hFlip = 1 },
    {.tileId = 27, .vFlip = 0, .hFlip = 1 },
  }
};

void
Object_CreateLowPlatform(Object *object) {
  object->hitbox = Bounds_Of(40, 16, 40, 9);
  object->tiles = &platform;
  Object_SetViewboxFromTiles(object);
}
