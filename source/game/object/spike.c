
#include <game/object.h>

typedef struct Properties {
  Direction direction;
} align4 Properties;

static const GBA_TileMapRef spikes[] = {
  [DIRECTION_LEFT] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 43, .vFlip = 1, .hFlip = 0 },
      { .tileId = 44, .vFlip = 1, .hFlip = 0 },
      { .tileId = 43, .vFlip = 0, .hFlip = 0 },
      { .tileId = 44, .vFlip = 0, .hFlip = 0 },
    }
  },
  [DIRECTION_RIGHT] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 44, .vFlip = 1, .hFlip = 1 },
      { .tileId = 43, .vFlip = 1, .hFlip = 1 },
      { .tileId = 44, .vFlip = 0, .hFlip = 1 },
      { .tileId = 43, .vFlip = 0, .hFlip = 1 },
    }
  },
  [DIRECTION_UP] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 17, .vFlip = 0, .hFlip = 0 },
      { .tileId = 17, .vFlip = 0, .hFlip = 1 },
      { .tileId = 25, .vFlip = 0, .hFlip = 0 },
      { .tileId = 25, .vFlip = 0, .hFlip = 1 },
    }
  },
  [DIRECTION_DOWN] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 25, .vFlip = 1, .hFlip = 0 },
      { .tileId = 25, .vFlip = 1, .hFlip = 1 },
      { .tileId = 17, .vFlip = 1, .hFlip = 0 },
      { .tileId = 17, .vFlip = 1, .hFlip = 1 },
    }
  }
};

static void
Object_ProtoDraw(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  Properties *props = Object_GetProperties(object);
  Direction direction = props->direction;

  const GBA_TileMapRef *tiles = &spikes[direction];
  GBA_TileMapRef_Blit(target, tx, ty, tiles);
}

bool
Object_CreateSpike(
    Object *object,
    Direction direction)
{
  static const Prototype prototype = {
    .draw = Object_ProtoDraw,
  };

  Bounds hitbox  = Bounds_Of(8, 8, 8, 8);
  Bounds viewbox = Bounds_Of(8, 8, 8, 8);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = true;
  object->proto = &prototype;

  Properties *props = Object_GetProperties(object);
  props->direction = direction;

  return true;
}
