
#include <game/object.h>

typedef struct Properties {
  bool offset;
  int width;
} align4 Properties;

bool
Object_CreateDisk(
    Object *object,
    int width)
{
  if (width < 1) {
    return false;
  }

  int x = width * 8;

  Bounds hitbox  = Bounds_Of(x, 8, x, 4);
  Bounds viewbox = Bounds_Of(x, 8, x, 4);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = false;
  object->type = OBJECT_TYPE_DISK;

  Properties *props = Object_GetProperties(object);
  props->offset = false;
  props->width = width;

  return true;
}

bool
Object_CreateOffsetDisk(
    Object *object,
    Direction direction,
    int width)
{
  if (width < 1) {
    return false;
  }

  int x = width * 8;

  Bounds hitbox  = Bounds_Of(x, 8, x, 4);
  Bounds viewbox = Bounds_Of(x, 8, x, 4);

  const Vector *delta = Vector_FromDirection(direction);

  hitbox.center.y += 4 * delta->y;
  viewbox.center.y += 4 * delta->y;

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = false;
  object->type = OBJECT_TYPE_DISK;

  Properties *props = Object_GetProperties(object);
  props->offset = true;
  props->width = width;

  return true;
}

static const GBA_TileMapRef disks[] = {
  {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 16, .vFlip = 0, .hFlip = 0 },
      { .tileId = 16, .vFlip = 0, .hFlip = 1 },
      { .tileId = 24, .vFlip = 0, .hFlip = 0 },
      { .tileId = 24, .vFlip = 0, .hFlip = 1 },
    }
  },
  {
    .width = 2, .height = 1,
    .tiles = (GBA_Tile[]) {
      { .tileId = 32, .vFlip = 0, .hFlip = 0 },
      { .tileId = 32, .vFlip = 0, .hFlip = 1 },
    }
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

  Properties *props = Object_GetProperties(object);

  const GBA_TileMapRef *tiles = props->offset ? &disks[1] : &disks[0];
  int width = props->width;

  for (int i = 0; i < width; i++) {
    GBA_TileMapRef_Blit(target, tx + i * tiles->width, ty, tiles);
  }
}
