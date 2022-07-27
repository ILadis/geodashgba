
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
  object->tiles = &platform;
}

void
Object_DrawColumn(
    Object *object,
    Camera *camera,
    int x)
{
  Vector *position = Camera_GetPosition(camera);
  int cy = position->y >> 3;

  const GBA_TileMapRef *tileMap = object->tiles;
  int height = tileMap->height;
  int width = tileMap->width;

  int oy = object->position.y;
  int ox = object->position.x;

  int dx = x - ox;
  if (dx < 0 || dx >= width) return;

  int sy = Math_max(oy, cy);
  int ey = Math_min(oy + height, cy + 21);

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  for (int y = sy; y < ey; y++) {
    int i = (y - oy) * width + dx;

    GBA_Tile *tile = &tileMap->tiles[i];
    GBA_TileMapRef_BlitTile(&target, x, y, tile);
  }
}

void
Object_DrawRow(
    Object *object,
    Camera *camera,
    int y)
{
  Vector *position = Camera_GetPosition(camera);
  int cx = position->x >> 3;

  const GBA_TileMapRef *tileMap = object->tiles;
  int height = tileMap->height;
  int width = tileMap->width;

  int oy = object->position.y;
  int ox = object->position.x;

  int dy = y - oy;
  if (dy < 0 || dy >= height) return;

  int sx = Math_max(ox, cx);
  int ex = Math_min(ox + width, cx + 31);

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  for (int x = sx; x < ex; x++) {
    int i = dy * width + (x - ox);

    GBA_Tile *tile = &tileMap->tiles[i];
    GBA_TileMapRef_BlitTile(&target, x, y, tile);
  }
}
