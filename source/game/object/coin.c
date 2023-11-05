
#include <game/object.h>
#include <game/cube.h>
#include <game/progress.h>

typedef struct Properties {
  int index;
  bool collected;
} align4 Properties;

bool
Object_CreateCoin(
    Object *object,
    int index)
{
  Bounds hitbox  = Bounds_Of(8, 8, 8, 8);
  Bounds viewbox = Bounds_Of(8, 8, 8, 8);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = false;
  object->deadly = false;
  object->type = OBJECT_TYPE_COIN;

  Properties *props = Object_GetProperties(object);
  props->index = index;
  props->collected = false;

  return true;
}

bool
Object_HitCoin(
    Object *object,
    Shape *shape)
{
  Properties *props = Object_GetProperties(object);

  // TODO workaround to prevent shadow from collecting coins
  if (Cube_IsShadowShape(shape)) {
    return true;
  }

  // TODO add circular shape

  bool collected = props->collected;
  if (!collected) {
    props->collected = true;

    Progress *progress = Progress_GetInstance();
    Progress_SetCollectedCoin(progress, props->index);
  }

  return true;
}

static const GBA_TileMapRef coin[] = {
  // coin frames
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 192, .vFlip = 0, .hFlip = 0 },
      { .tileId = 193, .vFlip = 0, .hFlip = 0 },
      { .tileId = 194, .vFlip = 0, .hFlip = 0 },
      { .tileId = 195, .vFlip = 0, .hFlip = 0 },
    }
  },
  {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 196, .vFlip = 0, .hFlip = 0 },
      { .tileId = 197, .vFlip = 0, .hFlip = 0 },
      { .tileId = 198, .vFlip = 0, .hFlip = 0 },
      { .tileId = 199, .vFlip = 0, .hFlip = 0 },
    }
  },
  {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 200, .vFlip = 0, .hFlip = 0 },
      { .tileId = 201, .vFlip = 0, .hFlip = 0 },
      { .tileId = 202, .vFlip = 0, .hFlip = 0 },
      { .tileId = 203, .vFlip = 0, .hFlip = 0 },
    }
  },
  { // collected (empty)
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[4]) {{0}},
  }
};

bool
Object_AnimateCoin(
    Object *object,
    GBA_TileMapRef *target,
    int frame)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  Properties *props = Object_GetProperties(object);
  int index = props->collected ? 3 : (frame / 8) % 3;

  GBA_TileMapRef_Blit(target, tx, ty, &coin[index]);

  return true;
}

void
Object_DrawCoin(
    Object *object,
    GBA_TileMapRef *target)
{
  Object_AnimateCoin(object, target, 0);
}
