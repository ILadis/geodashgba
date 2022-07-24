
#include <game/object.h>

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
  object->viewport = Bounds_Of(8, 8, 10,10);
  object->tiles = &box;
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
  object->viewport = Bounds_Of(8, 20, 10, 22);
  object->tiles = &blockWpole;
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
  object->viewport = Bounds_Of(40, 16, 42, 10);
  object->tiles = &platform;
}

void
Object_DrawColumn(
    Object *object,
    Camera *camera,
    int x)
{
  if (!Camera_InViewport(camera, &object->viewport)) return;

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
    Camera *camera,
    int y)
{
  if (!Camera_InViewport(camera, &object->viewport)) return;

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  const GBA_TileMapRef *tileMap = object->tiles;

  int dy = y - object->position.y;
  if (dy < 0) return;

  int width = tileMap->width;
  int height = tileMap->height;
  if (dy >= height) return;

  for (int x = 0; x < width; x++) {
    int i = dy * width + x;

    GBA_Tile *tile = &tileMap->tiles[i];
    GBA_TileMapRef_BlitTile(&target, object->position.x + x, y, tile);
  }
}
