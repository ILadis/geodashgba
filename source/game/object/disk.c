
#include <game/object.h>

bool
Object_CreateDisk(Object *object) {
  Bounds hitbox  = Bounds_Of(8, 8, 8, 4);
  Bounds viewbox = Bounds_Of(8, 8, 8, 4);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = false;
  object->type = TYPE_DISK;

  return true;
}

static const GBA_TileMapRef disk = {
  .width = 2, .height = 2,
  .tiles = (GBA_Tile[]) {
    { .tileId = 16, .vFlip = 0, .hFlip = 0 },
    { .tileId = 16, .vFlip = 0, .hFlip = 1 },
    { .tileId = 24, .vFlip = 0, .hFlip = 0 },
    { .tileId = 24, .vFlip = 0, .hFlip = 1 },
  }
};

void
Object_DrawDisk(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  GBA_TileMapRef_Blit(target, tx, ty, &disk);
}
