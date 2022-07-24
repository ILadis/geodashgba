
#include <game/object.h>

static const GBA_TileMapRef block = (GBA_TileMapRef) {
  .width = 2,
  .height = 2,
  .tiles = (GBA_Tile[]) {
    {.tileId = 19, .vFlip = 0, .hFlip = 0 },
    {.tileId = 20, .vFlip = 0, .hFlip = 0 },
    {.tileId = 27, .vFlip = 0, .hFlip = 0 },
    {.tileId = 28, .vFlip = 0, .hFlip = 0 },
  }
};

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
    {.tileId = 19, .vFlip = 0, .hFlip = 0 },
    {.tileId = 20, .vFlip = 0, .hFlip = 0 },
    {.tileId = 27, .vFlip = 0, .hFlip = 0 },
    {.tileId = 28, .vFlip = 0, .hFlip = 0 },
  }
};

static const GBA_TileMapRef box = (GBA_TileMapRef) {
  .width = 2,
  .height = 2,
  .tiles = (GBA_Tile[]) {
    {.tileId = 21, .vFlip = 0, .hFlip = 0 },
    {.tileId = 22, .vFlip = 0, .hFlip = 0 },
    {.tileId = 29, .vFlip = 0, .hFlip = 0 },
    {.tileId = 30, .vFlip = 0, .hFlip = 0 },
  }
};

static const GBA_TileMapRef pole = (GBA_TileMapRef) {
  .width = 2,
  .height = 3,
  .tiles = (GBA_Tile[]) {
    {.tileId = 37, .vFlip = 0, .hFlip = 0 },
    {.tileId = 38, .vFlip = 0, .hFlip = 0 },
    {.tileId = 45, .vFlip = 0, .hFlip = 0 },
    {.tileId = 46, .vFlip = 0, .hFlip = 0 },
    {.tileId = 53, .vFlip = 0, .hFlip = 0 },
    {.tileId = 54, .vFlip = 0, .hFlip = 0 },
  }
};

void
Object_CreateBlock(Object *object) {
  object->hitbox = Bounds_Of(8, 8, 8, 9);
  object->tiles = &block;
}

void
Object_CreateBlockWithPole(Object *object) {
  object->hitbox = Bounds_Of(8, 32, 8, 9);
  object->tiles = &blockWpole;
}

void
Object_CreateBox(Object *object) {
  object->hitbox = Bounds_Of(8, 8, 8, 9);
  object->tiles = &box;
}

void
Object_CreatePole(Object *object) {
  object->hitbox = Bounds_Of(0, 0, 0, 0);
  object->tiles = &pole;
}

void
Object_DrawColumn(
    Object *object,
    int x)
{
  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  const GBA_TileMapRef *tileMap = object->tiles;

  int dx = x - object->position.x;
  if (dx < 0) return;

  int height = tileMap->height;
  int width = tileMap->width;
  if (dx >= width) return;

  for (int y = 0; y < height; y++) {
    int i = y * width + dx;

    GBA_Tile *tile = &tileMap->tiles[i];
    GBA_TileMapRef_BlitTile(&target, x, object->position.y + y, tile);
  }
}

void
Object_DrawRow(
    Object *object,
    int y)
{
  
}
