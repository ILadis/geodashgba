
#include <game/object.h>
#include <game/cube.h>

typedef struct Properties {
  bool collected;
  int frame;
} align4 Properties;

bool
Object_CreateCoin(Object *object) {
  Bounds hitbox  = Bounds_Of(8, 8, 8, 8);
  Bounds viewbox = Bounds_Of(8, 8, 8, 8);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = false;
  object->deadly = false;
  object->type = TYPE_COIN;

  Properties *props = Object_GetProperties(object);
  props->collected = false;
  props->frame = 0;

  return true;
}

bool
Object_HitCoin(
    Object *object,
    Shape *shape)
{
  Properties *props = Object_GetProperties(object);

  Cube *cube = Cube_GetInstance();
  // TODO workaround to prevent shadow from collecting coins
  if (&cube->shape != shape) {
    return true;
  }

  // TODO add circular shape

  bool collected = props->collected;
  if (!collected) {
    props->collected = true;
  }

  return true;
}

static const GBA_TileMapRef coin[] = {
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 192, .vFlip = 0, .hFlip = 0 },
      { .tileId = 193, .vFlip = 0, .hFlip = 0 },
      { .tileId = 194, .vFlip = 0, .hFlip = 0 },
      { .tileId = 195, .vFlip = 0, .hFlip = 0 },
    }
  }, {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 196, .vFlip = 0, .hFlip = 0 },
      { .tileId = 197, .vFlip = 0, .hFlip = 0 },
      { .tileId = 198, .vFlip = 0, .hFlip = 0 },
      { .tileId = 199, .vFlip = 0, .hFlip = 0 },
    }
  }, {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 200, .vFlip = 0, .hFlip = 0 },
      { .tileId = 201, .vFlip = 0, .hFlip = 0 },
      { .tileId = 202, .vFlip = 0, .hFlip = 0 },
      { .tileId = 203, .vFlip = 0, .hFlip = 0 },
    }
  }
};

void
Object_DrawCoin(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  Properties *props = Object_GetProperties(object);
  int index = (props->frame / 8) % 3;

  GBA_TileMapRef_Blit(target, tx, ty, &coin[index]);
}

bool
Object_AnimateCoin(
    Object *object,
    GBA_TileMapRef *target)
{
  Properties *props = Object_GetProperties(object);
  props->frame++;

  Object_DrawCoin(object, target);

  return true;
}
