
#include <game/object.h>

typedef struct Properties {
  int height;
} align4 Properties;

bool
Object_CreateGoalWall(
    Object *object,
    int height)
{
  if ( height < 1) {
    return false;
  }

  int y = height * 8;

  Bounds hitbox  = Bounds_Of(4, y, 4, y);
  Bounds viewbox = Bounds_Of(4, y, 4, y);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = false;
  object->type = TYPE_GOAL_WALL;

  Properties *props = Object_GetProperties(object);
  props->height = height;

  return true;
}

static const GBA_TileMapRef wall = {
  .width = 1, .height = 2,
  .tiles = (GBA_Tile[]) {
    { .tileId = 27, .vFlip = 0, .hFlip = 0 },
    { .tileId = 27, .vFlip = 1, .hFlip = 0 },
  }
};

void
Object_DrawGoalWall(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  Properties *props = Object_GetProperties(object);

  const GBA_TileMapRef *tiles = &wall;
  int height = props->height;

  for (int i = 0; i < height; i++) {
    GBA_TileMapRef_Blit(target, tx, ty + i * tiles->height, tiles);
  }
}
